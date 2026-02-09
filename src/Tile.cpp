#include "Tile.h"

const char* GetTileName(TileType type) {
    switch (type) {
        case TileType::Empty: return "Empty";
        case TileType::Path:  return "Path";
        case TileType::Road:  return "Road";
        case TileType::Track: return "Track";
        default:              return "Unknown";
    }
}

Color GetTileColor(TileType type) {
    switch (type) {
        case TileType::Path:  return LIME;
        case TileType::Road:  return GRAY;
        case TileType::Track: return BROWN;
        default:              return BLANK;
    }
}

int GetTileWidth(TileType type) {
    switch (type) {
        case TileType::Road: return 2;
        default: return 1;
    }
}

int GetTileHeight(TileType type) {
    switch (type) {
        case TileType::Road: return 2;
        default: return 1;
    }
}

int GetTileTextureKey(TileType type, uint8_t connections) {
    return (static_cast<int>(type) << 8) | connections;
}

TileShape GetTileShape(uint8_t connections) {
    int count = 0;
    if (connections & CONN_UP) count++;
    if (connections & CONN_RIGHT) count++;
    if (connections & CONN_DOWN) count++;
    if (connections & CONN_LEFT) count++;

    switch (count) {
        case 0: return TileShape::Straight;  // Default to straight when no connections
        case 1: return TileShape::DeadEnd;
        case 2: {
            // Check if opposite sides (straight) or adjacent (corner)
            bool horizontal = (connections & CONN_LEFT) && (connections & CONN_RIGHT);
            bool vertical = (connections & CONN_UP) && (connections & CONN_DOWN);
            return (horizontal || vertical) ? TileShape::Straight : TileShape::Corner;
        }
        case 3: return TileShape::TJunction;
        case 4: return TileShape::Cross;
        default: return TileShape::Single;
    }
}

float GetTileRotation(uint8_t connections) {
    TileShape shape = GetTileShape(connections);

    switch (shape) {
        case TileShape::Straight:
            // Base texture is horizontal (left-right)
            if (connections & CONN_UP) return 90.0f;  // Vertical
            return 0.0f;  // Horizontal (default)

        case TileShape::Corner:
            // Base texture connects RIGHT and DOWN, rotated 180° to fix orientation
            if ((connections & CONN_RIGHT) && (connections & CONN_DOWN)) return 180.0f;
            if ((connections & CONN_DOWN) && (connections & CONN_LEFT)) return 270.0f;
            if ((connections & CONN_LEFT) && (connections & CONN_UP)) return 0.0f;
            if ((connections & CONN_UP) && (connections & CONN_RIGHT)) return 90.0f;
            return 0.0f;

        case TileShape::TJunction:
            // Base texture is missing UP connection (connects LEFT, RIGHT, DOWN)
            if (!(connections & CONN_UP)) return 0.0f;    // T pointing down
            if (!(connections & CONN_RIGHT)) return 90.0f;  // T pointing left
            if (!(connections & CONN_DOWN)) return 180.0f;  // T pointing up
            if (!(connections & CONN_LEFT)) return 270.0f;  // T pointing right
            return 0.0f;

        case TileShape::DeadEnd:
            // Base texture connects DOWN only
            if (connections & CONN_DOWN) return 0.0f;
            if (connections & CONN_LEFT) return 90.0f;
            if (connections & CONN_UP) return 180.0f;
            if (connections & CONN_RIGHT) return 270.0f;
            return 0.0f;

        default:
            return 0.0f;
    }
}

bool CanTilesConnect(TileType a, TileType b) {
    if (a == TileType::Empty || b == TileType::Empty) return false;
    // Same types always connect
    if (a == b) return true;
    // Road and Path can connect (sidewalk next to road)
    if ((a == TileType::Road && b == TileType::Path) ||
        (a == TileType::Path && b == TileType::Road)) return true;
    return false;
}

int TileTextures::GetShapeKey(TileType type, TileShape shape) const {
    return (static_cast<int>(type) << 4) | static_cast<int>(shape);
}

void TileTextures::Load() {
    // Path (sidewalk) - single texture
    textures[GetShapeKey(TileType::Path, TileShape::Single)] = LoadTexture("resources/sidewalk.png");

    // Road base textures (one per shape, rotated as needed)
    textures[GetShapeKey(TileType::Road, TileShape::Straight)] = LoadTexture("resources/road2x2Horizontal.png");
    textures[GetShapeKey(TileType::Road, TileShape::Corner)] = LoadTexture("resources/road2x2TurnRightDown.png");
    textures[GetShapeKey(TileType::Road, TileShape::TJunction)] = LoadTexture("resources/road2x2TDown.png");
    textures[GetShapeKey(TileType::Road, TileShape::Cross)] = LoadTexture("resources/road2x2Cross.png");
    textures[GetShapeKey(TileType::Road, TileShape::DeadEnd)] = LoadTexture("resources/road2x2EndDown.png");

    // Track base textures
    textures[GetShapeKey(TileType::Track, TileShape::Straight)] = LoadTexture("resources/railHorizontal.png");
    textures[GetShapeKey(TileType::Track, TileShape::Corner)] = LoadTexture("resources/railTurnRightDown.png");

    loaded = true;
}

void TileTextures::Unload() {
    for (auto& [key, texture] : textures) {
        UnloadTexture(texture);
    }
    textures.clear();
    loaded = false;
}

bool TileTextures::HasTexture(TileType type, TileShape shape) const {
    return textures.find(GetShapeKey(type, shape)) != textures.end();
}

Texture2D TileTextures::Get(TileType type, TileShape shape) const {
    int key = GetShapeKey(type, shape);
    auto it = textures.find(key);
    if (it != textures.end()) {
        return it->second;
    }
    // Fallback: try straight shape
    it = textures.find(GetShapeKey(type, TileShape::Straight));
    if (it != textures.end()) {
        return it->second;
    }
    return Texture2D{};
}
