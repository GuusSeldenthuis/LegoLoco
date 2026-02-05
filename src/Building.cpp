#include "Building.h"

Building CreateBuilding(BuildingType type, int gridX, int gridY) {
    Building b;
    b.type = type;
    b.gridX = gridX;
    b.gridY = gridY;

    switch (type) {
        case BuildingType::RedHouse:
            b.width = 2;
            b.height = 2;
            // Texture is 48x58, footprint is 32x32
            // Center horizontally: (48-32)/2 = 8px offset left
            // Roof extends above: 58-32 = 26px offset up
            b.renderOffsetX = -8;
            b.renderOffsetY = -26;
            break;
        default:
            b.width = 1;
            b.height = 1;
            b.renderOffsetX = 0;
            b.renderOffsetY = 0;
            break;
    }

    return b;
}

const char* GetBuildingName(BuildingType type) {
    switch (type) {
        case BuildingType::None:     return "None";
        case BuildingType::RedHouse: return "Red House";
        default:                     return "Unknown";
    }
}

void BuildingTextures::Load() {
    textures[BuildingType::RedHouse] = LoadTexture("resources/redHouse.png");
    loaded = true;
}

void BuildingTextures::Unload() {
    for (auto& [type, texture] : textures) {
        UnloadTexture(texture);
    }
    textures.clear();
    loaded = false;
}

bool BuildingTextures::HasTexture(BuildingType type) const {
    return textures.find(type) != textures.end();
}

Texture2D BuildingTextures::Get(BuildingType type) const {
    auto it = textures.find(type);
    if (it != textures.end()) {
        return it->second;
    }
    return Texture2D{};
}
