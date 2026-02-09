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

// Helper to get anchor position for a tile (returns itself if anchor or empty)
Vector2 World::GetAnchorPos(int x, int y) const {
    if (x < 0 || x >= cols || y < 0 || y >= rows) return {(float)x, (float)y};
    const Tile& t = tiles[y][x];
    if (t.isAnchor) return {(float)x, (float)y};
    return {(float)(x + t.anchorOffsetX), (float)(y + t.anchorOffsetY)};
}

// Clear a multi-tile starting from any cell
void World::ClearMultiTile(int x, int y) {
    if (x < 0 || x >= cols || y < 0 || y >= rows) return;
    if (tiles[y][x].type == TileType::Empty) return;

    // Find anchor
    Vector2 anchor = GetAnchorPos(x, y);
    int ax = (int)anchor.x;
    int ay = (int)anchor.y;

    if (ax < 0 || ax >= cols || ay < 0 || ay >= rows) return;

    TileType type = tiles[ay][ax].type;
    int w = GetTileWidth(type);
    int h = GetTileHeight(type);

    // Clear all cells of this multi-tile
    for (int dy = 0; dy < h; dy++) {
        for (int dx = 0; dx < w; dx++) {
            int cx = ax + dx;
            int cy = ay + dy;
            if (cx >= 0 && cx < cols && cy >= 0 && cy < rows) {
                tiles[cy][cx] = Tile{};
            }
        }
    }
}

void World::SetTile(int x, int y, TileType type) {
    int w = GetTileWidth(type);
    int h = GetTileHeight(type);

    // Check bounds for multi-tile
    if (x < 0 || y < 0 || x + w > cols || y + h > rows) return;

    // For empty type, clear the tile at this position (handling multi-tiles)
    if (type == TileType::Empty) {
        ClearMultiTile(x, y);
        UpdateAllConnections();
        return;
    }

    // Clear any existing tiles in the footprint
    for (int dy = 0; dy < h; dy++) {
        for (int dx = 0; dx < w; dx++) {
            ClearMultiTile(x + dx, y + dy);
        }
    }

    // Place the new tile
    for (int dy = 0; dy < h; dy++) {
        for (int dx = 0; dx < w; dx++) {
            Tile& t = tiles[y + dy][x + dx];
            t.type = type;
            t.connections = CONN_NONE;
            t.isAnchor = (dx == 0 && dy == 0);
            t.anchorOffsetX = -dx;
            t.anchorOffsetY = -dy;
        }
    }

    UpdateAllConnections();
}

uint8_t World::CalculateConnections(int x, int y) const {
    if (x < 0 || x >= cols || y < 0 || y >= rows) return CONN_NONE;

    const Tile& tile = tiles[y][x];
    if (tile.type == TileType::Empty || tile.type == TileType::Path) return CONN_NONE;
    if (!tile.isAnchor) return CONN_NONE;  // Only anchors track connections

    TileType myType = tile.type;
    int w = GetTileWidth(myType);
    int h = GetTileHeight(myType);

    uint8_t connections = CONN_NONE;

    // Check UP edge (row y-1, columns x to x+w-1)
    for (int dx = 0; dx < w; dx++) {
        int checkY = y - 1;
        int checkX = x + dx;
        if (checkY >= 0 && checkX < cols) {
            TileType neighborType = tiles[checkY][checkX].type;
            if (CanTilesConnect(myType, neighborType)) {
                connections |= CONN_UP;
                break;
            }
        }
    }

    // Check DOWN edge (row y+h, columns x to x+w-1)
    for (int dx = 0; dx < w; dx++) {
        int checkY = y + h;
        int checkX = x + dx;
        if (checkY < rows && checkX < cols) {
            TileType neighborType = tiles[checkY][checkX].type;
            if (CanTilesConnect(myType, neighborType)) {
                connections |= CONN_DOWN;
                break;
            }
        }
    }

    // Check LEFT edge (column x-1, rows y to y+h-1)
    for (int dy = 0; dy < h; dy++) {
        int checkY = y + dy;
        int checkX = x - 1;
        if (checkX >= 0 && checkY < rows) {
            TileType neighborType = tiles[checkY][checkX].type;
            if (CanTilesConnect(myType, neighborType)) {
                connections |= CONN_LEFT;
                break;
            }
        }
    }

    // Check RIGHT edge (column x+w, rows y to y+h-1)
    for (int dy = 0; dy < h; dy++) {
        int checkY = y + dy;
        int checkX = x + w;
        if (checkX < cols && checkY < rows) {
            TileType neighborType = tiles[checkY][checkX].type;
            if (CanTilesConnect(myType, neighborType)) {
                connections |= CONN_RIGHT;
                break;
            }
        }
    }

    return connections;
}

void World::UpdateTileConnections(int x, int y) {
    if (x < 0 || x >= cols || y < 0 || y >= rows) return;
    if (!tiles[y][x].isAnchor) return;
    tiles[y][x].connections = CalculateConnections(x, y);
}

void World::UpdateAllConnections() {
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            if (tiles[y][x].isAnchor && tiles[y][x].type != TileType::Empty) {
                tiles[y][x].connections = CalculateConnections(x, y);
            }
        }
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
            const Tile& tile = tiles[y][x];

            // Only render anchor tiles (skip non-anchor parts of multi-tiles)
            if (tile.type == TileType::Empty || !tile.isAnchor) continue;

            Vector2 pos = WorldToScreen(x, y, camera.offset, camera.zoom);
            int w = GetTileWidth(tile.type);
            int h = GetTileHeight(tile.type);
            float renderW = TILE_SIZE * w * camera.zoom;
            float renderH = TILE_SIZE * h * camera.zoom;

            // Get shape and rotation based on connections
            TileShape shape = (tile.type == TileType::Path) ? TileShape::Single : GetTileShape(tile.connections);
            float rotation = GetTileRotation(tile.connections);

            if (textures.HasTexture(tile.type, shape)) {
                Texture2D tex = textures.Get(tile.type, shape);
                Rectangle source = { 0, 0, (float)tex.width, (float)tex.height };
                // For rotation, we need to position dest at center and use origin
                float centerX = pos.x + renderW / 2;
                float centerY = pos.y + renderH / 2;
                Rectangle dest = { centerX, centerY, renderW, renderH };
                Vector2 origin = { renderW / 2, renderH / 2 };
                DrawTexturePro(tex, source, dest, origin, rotation, WHITE);
            } else {
                Color color = GetTileColor(tile.type);
                DrawRectangle((int)pos.x, (int)pos.y, (int)renderW, (int)renderH, color);
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

void World::RebuildPathGraph() {
    pathGraph.Build(tiles, rows, cols);
}

void World::RenderPathDebug(GameCamera& camera) {
    pathGraph.RenderDebug(camera);
}
