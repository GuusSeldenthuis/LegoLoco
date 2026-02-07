#include "World.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

World::World(int rows, int cols) : rows(rows), cols(cols) {
    tiles.resize(rows, std::vector<Tile>(cols));
}

void World::Clear() {
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            tiles[y][x] = Tile{};
        }
    }
    buildings.clear();
}

bool World::Save(const std::string& filepath) const {
    // Ensure directory exists
    std::filesystem::path path(filepath);
    std::filesystem::create_directories(path.parent_path());

    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    file << "{\n";
    file << "  \"rows\": " << rows << ",\n";
    file << "  \"cols\": " << cols << ",\n";
    file << "  \"tiles\": [\n";

    bool first = true;
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            const Tile& tile = tiles[y][x];
            if (tile.type != TileType::Empty) {
                if (!first) file << ",\n";
                first = false;
                file << "    {\"x\": " << x
                     << ", \"y\": " << y
                     << ", \"type\": " << static_cast<int>(tile.type) << "}";
            }
        }
    }

    file << "\n  ]\n";
    file << "}\n";

    return true;
}

bool World::Load(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    // Simple JSON parsing for our specific format
    Clear();

    // Parse rows
    size_t rowsPos = content.find("\"rows\":");
    if (rowsPos != std::string::npos) {
        int loadedRows = std::stoi(content.substr(rowsPos + 7));
        if (loadedRows != rows) {
            // World size mismatch - we keep current size
        }
    }

    // Parse cols
    size_t colsPos = content.find("\"cols\":");
    if (colsPos != std::string::npos) {
        int loadedCols = std::stoi(content.substr(colsPos + 7));
        if (loadedCols != cols) {
            // World size mismatch - we keep current size
        }
    }

    // Parse tiles array
    size_t tilesStart = content.find("\"tiles\":");
    if (tilesStart == std::string::npos) return false;

    size_t pos = tilesStart;
    while ((pos = content.find("{\"x\":", pos)) != std::string::npos) {
        // Parse x
        size_t xStart = pos + 5;
        int x = std::stoi(content.substr(xStart));

        // Parse y
        size_t yPos = content.find("\"y\":", pos);
        int y = std::stoi(content.substr(yPos + 4));

        // Parse type
        size_t typePos = content.find("\"type\":", pos);
        int type = std::stoi(content.substr(typePos + 7));

        // Set tile if within bounds
        if (x >= 0 && x < cols && y >= 0 && y < rows) {
            tiles[y][x].type = static_cast<TileType>(type);
        }

        pos++;
    }

    return true;
}

void World::SetTile(int x, int y, TileType type) {
    if (x >= 0 && x < cols && y >= 0 && y < rows) {
        tiles[y][x].type = type;
    }
}

Tile World::GetTile(int x, int y) const {
    if (x >= 0 && x < cols && y >= 0 && y < rows) {
        return tiles[y][x];
    }
    return Tile{};
}

void World::Render(GameCamera& camera, TileTextures& textures) {
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            Vector2 pos = WorldToScreen(x, y, camera.offset, camera.zoom);
            TileType type = tiles[y][x].type;

            if (type != TileType::Empty) {
                float tileSize = TILE_SIZE * camera.zoom;
                if (textures.HasTexture(type)) {
                    Texture2D tex = textures.Get(type);
                    Rectangle source = { 0, 0, (float)tex.width, (float)tex.height };
                    Rectangle dest = { pos.x, pos.y, tileSize, tileSize };
                    DrawTexturePro(tex, source, dest, {0, 0}, 0.0f, WHITE);
                } else {
                    Color color = GetTileColor(type);
                    DrawRectangle((int)pos.x, (int)pos.y, (int)tileSize, (int)tileSize, color);
                }
            }
        }
    }
}

void World::RenderBuildings(GameCamera& camera, BuildingTextures& textures) {
    // Sort indices by Y position so buildings further back render first
    std::vector<int> order(buildings.size());
    for (int i = 0; i < (int)buildings.size(); i++) order[i] = i;
    std::sort(order.begin(), order.end(), [&](int a, int b) {
        return buildings[a].gridY < buildings[b].gridY;
    });

    for (int idx : order) {
        const Building& b = buildings[idx];
        if (b.type == BuildingType::None) continue;

        Vector2 pos = WorldToScreen(b.gridX, b.gridY, camera.offset, camera.zoom);

        // Apply render offset (scaled by zoom)
        pos.x += b.renderOffsetX * camera.zoom;
        pos.y += b.renderOffsetY * camera.zoom;

        if (textures.HasTexture(b.type)) {
            Texture2D tex = textures.Get(b.type);
            Rectangle source = { 0, 0, (float)tex.width, (float)tex.height };
            Rectangle dest = { pos.x, pos.y, tex.width * camera.zoom, tex.height * camera.zoom };
            DrawTexturePro(tex, source, dest, {0, 0}, 0.0f, WHITE);
        }
    }
}

bool World::CanPlace(const Placeable& placeable) const {
    // Check bounds
    if (placeable.gridX < 0 || placeable.gridY < 0 ||
        placeable.gridX + placeable.width > cols ||
        placeable.gridY + placeable.height > rows) {
        return false;
    }

    // Check overlap with existing buildings
    for (const Building& existing : buildings) {
        if (existing.type == BuildingType::None) continue;

        bool overlapX = placeable.gridX < existing.gridX + existing.width &&
                        placeable.gridX + placeable.width > existing.gridX;
        bool overlapY = placeable.gridY < existing.gridY + existing.height &&
                        placeable.gridY + placeable.height > existing.gridY;

        if (overlapX && overlapY) {
            return false;
        }
    }

    return true;
}

bool World::PlaceBuilding(BuildingType type, int gridX, int gridY) {
    Building b = CreateBuilding(type, gridX, gridY);

    if (!CanPlace(b)) {
        return false;
    }

    buildings.push_back(b);
    return true;
}

bool World::RemoveBuilding(int gridX, int gridY) {
    for (auto it = buildings.begin(); it != buildings.end(); ++it) {
        // Check if the point is within the building's footprint
        if (gridX >= it->gridX && gridX < it->gridX + it->width &&
            gridY >= it->gridY && gridY < it->gridY + it->height) {
            buildings.erase(it);
            return true;
        }
    }
    return false;
}

Building* World::GetBuildingAt(int gridX, int gridY) {
    for (Building& b : buildings) {
        if (gridX >= b.gridX && gridX < b.gridX + b.width &&
            gridY >= b.gridY && gridY < b.gridY + b.height) {
            return &b;
        }
    }
    return nullptr;
}
