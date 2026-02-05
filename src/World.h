#pragma once

#include "Tile.h"
#include "Building.h"
#include "Camera.h"
#include <vector>
#include <string>

class World {
private:
    int rows;
    int cols;
    std::vector<std::vector<Tile>> tiles;
    std::vector<Building> buildings;

public:
    World(int rows, int cols);

    void SetTile(int x, int y, TileType type);
    Tile GetTile(int x, int y) const;
    void Render(GameCamera& camera, TileTextures& textures);
    void RenderBuildings(GameCamera& camera, BuildingTextures& textures);

    // Building management
    bool CanPlaceBuilding(const Building& building) const;
    bool PlaceBuilding(BuildingType type, int gridX, int gridY);
    bool RemoveBuilding(int gridX, int gridY);
    Building* GetBuildingAt(int gridX, int gridY);
    const std::vector<Building>& GetBuildings() const { return buildings; }

    bool Save(const std::string& filepath) const;
    bool Load(const std::string& filepath);
    void Clear();

    int GetRows() const { return rows; }
    int GetCols() const { return cols; }
};
