#pragma once

#include "Tile.h"
#include "Camera.h"
#include <vector>

class World {
private:
    int rows;
    int cols;
    std::vector<std::vector<Tile>> tiles;

public:
    World(int rows, int cols);

    void SetTile(int x, int y, TileType type, int orientation = 0);
    Tile GetTile(int x, int y) const;
    void Render(GameCamera& camera, TileTextures& textures);

    int GetRows() const { return rows; }
    int GetCols() const { return cols; }
};
