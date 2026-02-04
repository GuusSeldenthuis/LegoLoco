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
