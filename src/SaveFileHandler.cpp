#include "SaveFileHandler.h"
#include "World.h"
#include <fstream>
#include <sstream>
#include <filesystem>

bool SaveFileHandler::Save(const World& world, const std::string& filepath) const {
    // Ensure directory exists
    std::filesystem::path path(filepath);
    std::filesystem::create_directories(path.parent_path());

    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    int rows = world.GetRows();
    int cols = world.GetCols();

    file << "{\n";
    file << "  \"rows\": " << rows << ",\n";
    file << "  \"cols\": " << cols << ",\n";

    // Save tiles
    file << "  \"tiles\": [\n";
    bool first = true;
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            Tile tile = world.GetTile(x, y);
            if (tile.type != TileType::Empty) {
                if (!first) file << ",\n";
                first = false;
                file << "    {\"x\": " << x
                     << ", \"y\": " << y
                     << ", \"type\": " << static_cast<int>(tile.type)
                     << ", \"rotation\": " << tile.rotation << "}";
            }
        }
    }
    file << "\n  ],\n";

    // Save buildings
    file << "  \"buildings\": [\n";
    const auto& buildings = world.GetBuildings();
    for (size_t i = 0; i < buildings.size(); i++) {
        const Building& b = buildings[i];
        if (b.type == BuildingType::None) continue;
        if (i > 0) file << ",\n";
        file << "    {\"x\": " << b.gridX
             << ", \"y\": " << b.gridY
             << ", \"type\": " << static_cast<int>(b.type) << "}";
    }
    file << "\n  ]\n";

    file << "}\n";

    return true;
}

bool SaveFileHandler::Load(World& world, const std::string& filepath) const {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    world.Clear();

    // Parse tiles
    size_t tilesStart = content.find("\"tiles\":");
    size_t buildingsStart = content.find("\"buildings\":");

    // Determine where the tiles array ends
    size_t tilesEnd = (buildingsStart != std::string::npos) ? buildingsStart : content.size();

    if (tilesStart != std::string::npos) {
        size_t pos = tilesStart;
        while ((pos = content.find("{\"x\":", pos)) != std::string::npos && pos < tilesEnd) {
            size_t xStart = pos + 5;
            int x = std::stoi(content.substr(xStart));

            size_t yPos = content.find("\"y\":", pos);
            int y = std::stoi(content.substr(yPos + 4));

            size_t typePos = content.find("\"type\":", pos);
            int type = std::stoi(content.substr(typePos + 7));

            float rotation = 0.0f;
            size_t nextBrace = content.find("}", pos);
            size_t rotPos = content.find("\"rotation\":", pos);
            if (rotPos != std::string::npos && rotPos < nextBrace) {
                rotation = std::stof(content.substr(rotPos + 11));
            }

            if (x >= 0 && x < world.GetCols() && y >= 0 && y < world.GetRows()) {
                world.SetTileRaw(x, y, static_cast<TileType>(type), rotation);
            }

            pos++;
        }
    }

    // Parse buildings
    if (buildingsStart != std::string::npos) {
        size_t pos = buildingsStart;
        while ((pos = content.find("{\"x\":", pos)) != std::string::npos) {
            size_t xStart = pos + 5;
            int x = std::stoi(content.substr(xStart));

            size_t yPos = content.find("\"y\":", pos);
            int y = std::stoi(content.substr(yPos + 4));

            size_t typePos = content.find("\"type\":", pos);
            int type = std::stoi(content.substr(typePos + 7));

            world.PlaceBuilding(static_cast<BuildingType>(type), x, y);

            pos++;
        }
    }

    world.UpdateAllConnections();

    return true;
}
