#pragma once

#include "raylib.h"
#include <unordered_map>
#include <cstdint>

const int TILE_SIZE = 16;

// Connection directions as bitmask
enum TileConnection : uint8_t {
    CONN_NONE  = 0,
    CONN_UP    = 1,
    CONN_RIGHT = 2,
    CONN_DOWN  = 4,
    CONN_LEFT  = 8
};

enum class TileType {
    Empty,
    Path,
    Road,
    Track,
    TrackCorner
};

// Get tile dimensions (most tiles are 1x1, roads are 2x2)
int GetTileWidth(TileType type);
int GetTileHeight(TileType type);

struct Tile {
    TileType type = TileType::Empty;
    uint8_t connections = CONN_NONE;
    float rotation = 0.0f;  // Manual rotation for track pieces
    // For multi-tile types: true if this is the anchor (top-left), false if part of larger tile
    bool isAnchor = true;
    // For non-anchor cells: offset to the anchor cell
    int8_t anchorOffsetX = 0;
    int8_t anchorOffsetY = 0;
};

const char* GetTileName(TileType type);
Color GetTileColor(TileType type);

// Get texture key based on type and connections
int GetTileTextureKey(TileType type, uint8_t connections);

// Check if two tile types can connect to each other
bool CanTilesConnect(TileType a, TileType b);

// Shape types for tiles that can be rotated
enum class TileShape {
    Single,     // 1x1 no rotation (path, etc.)
    Straight,   // Connects 2 opposite sides
    Corner,     // Connects 2 adjacent sides
    TJunction,  // Connects 3 sides
    Cross,      // Connects all 4 sides
    DeadEnd     // Connects 1 side
};

// Get the shape and rotation for a given connection pattern
TileShape GetTileShape(uint8_t connections);
float GetTileRotation(uint8_t connections);

class TileTextures {
private:
    // Base textures for each tile type and shape
    // Key: (TileType << 4) | TileShape
    std::unordered_map<int, Texture2D> textures;
    bool loaded = false;

    int GetShapeKey(TileType type, TileShape shape) const;

public:
    void Load();
    void Unload();
    bool HasTexture(TileType type, TileShape shape) const;
    Texture2D Get(TileType type, TileShape shape) const;
    bool IsLoaded() const { return loaded; }
};
