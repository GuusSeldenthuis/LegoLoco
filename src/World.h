#pragma once

#include "Tile.h"
#include "Camera.h"
#include <vector>
#include <string>

class World {
private:
    int rows;
    int cols;
    std::vector<std::vector<Tile>> tiles;

public:
    World(int rows, int cols);

    void SetTile(int x, int y, TileType type);
    Tile GetTile(int x, int y) const;
    void Render(GameCamera& camera, TileTextures& textures);

    bool Save(const std::string& filepath) const;
    bool Load(const std::string& filepath);
    void Clear();

    int GetRows() const { return rows; }
    int GetCols() const { return cols; }
};
