#pragma once

#include "raylib.h"
#include <unordered_map>

const int TILE_SIZE = 16;

enum class TileType {
    Empty,
    Path,
    Road,
    Track
};

struct Tile {
    TileType type = TileType::Empty;
};

const char* GetTileName(TileType type);
Color GetTileColor(TileType type);

class TileTextures {
private:
    std::unordered_map<TileType, Texture2D> textures;
    bool loaded = false;

public:
    void Load();
    void Unload();
    bool HasTexture(TileType type) const;
    Texture2D Get(TileType type) const;
    bool IsLoaded() const { return loaded; }
};
