#include "World.h"
#include <fstream>
#include <sstream>
#include <filesystem>

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
        // For multi-tile types (2x2 like Road), remove overlapping tiles
        // A road at position (px, py) visually covers (px, py) to (px+1, py+1)
        // So placing at (x, y) conflicts with roads at (x-1, y), (x, y-1), (x-1, y-1)
        if (type == TileType::Road) {
            // Check left neighbor - its right side overlaps our left
            if (x > 0 && tiles[y][x-1].type == TileType::Road) {
                tiles[y][x-1].type = TileType::Empty;
            }
            // Check top neighbor - its bottom overlaps our top
            if (y > 0 && tiles[y-1][x].type == TileType::Road) {
                tiles[y-1][x].type = TileType::Empty;
            }
            // Check top-left neighbor - its bottom-right overlaps our top-left
            if (x > 0 && y > 0 && tiles[y-1][x-1].type == TileType::Road) {
                tiles[y-1][x-1].type = TileType::Empty;
            }
        }
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
                if (textures.HasTexture(type)) {
                    Texture2D tex = textures.Get(type);
                    Rectangle source = { 0, 0, (float)tex.width, (float)tex.height };
                    Rectangle dest = { pos.x, pos.y, tex.width * camera.zoom, tex.height * camera.zoom };
                    DrawTexturePro(tex, source, dest, {0, 0}, 0.0f, WHITE);
                } else {
                    float tileSize = TILE_SIZE * camera.zoom;
                    Color color = GetTileColor(type);
                    DrawRectangle((int)pos.x, (int)pos.y, (int)tileSize, (int)tileSize, color);
                }
            }
        }
    }
}

void World::RenderBuildings(GameCamera& camera, BuildingTextures& textures) {
    // Render buildings sorted by Y position for proper depth
    for (const Building& b : buildings) {
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

bool World::CanPlaceBuilding(const Building& building) const {
    // Check bounds
    if (building.gridX < 0 || building.gridY < 0 ||
        building.gridX + building.width > cols ||
        building.gridY + building.height > rows) {
        return false;
    }

    // Check overlap with existing buildings
    for (const Building& existing : buildings) {
        if (existing.type == BuildingType::None) continue;

        // Check if footprints overlap
        bool overlapX = building.gridX < existing.gridX + existing.width &&
                        building.gridX + building.width > existing.gridX;
        bool overlapY = building.gridY < existing.gridY + existing.height &&
                        building.gridY + building.height > existing.gridY;

        if (overlapX && overlapY) {
            return false;
        }
    }

    return true;
}

bool World::PlaceBuilding(BuildingType type, int gridX, int gridY) {
    Building b = CreateBuilding(type, gridX, gridY);

    if (!CanPlaceBuilding(b)) {
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
