#include "Tile.h"

const char* GetTileName(TileType type) {
    switch (type) {
        case TileType::Empty: return "Empty";
        case TileType::Grass: return "Grass";
        case TileType::Road:  return "Road";
        case TileType::Water: return "Water";
        case TileType::Track: return "Track";
        default:              return "Unknown";
    }
}

Color GetTileColor(TileType type) {
    switch (type) {
        case TileType::Grass: return LIME;
        case TileType::Road:  return DARKGRAY;
        case TileType::Water: return SKYBLUE;
        case TileType::Track: return BROWN;
        default:              return BLANK;
    }
}

void TileTextures::Load() {
    textures[TileType::Grass] = LoadTexture("resources/sidewalk.png");
    textures[TileType::Road] = LoadTexture("resources/roadHorizontal.png");
    textures[TileType::Track] = LoadTexture("resources/railHorizontal.png");
    loaded = true;
}

void TileTextures::Unload() {
    for (auto& [type, texture] : textures) {
        UnloadTexture(texture);
    }
    textures.clear();
    loaded = false;
}

bool TileTextures::HasTexture(TileType type) const {
    return textures.find(type) != textures.end();
}

Texture2D TileTextures::Get(TileType type) const {
    auto it = textures.find(type);
    if (it != textures.end()) {
        return it->second;
    }
    return Texture2D{};
}
