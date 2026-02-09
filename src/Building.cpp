#include "Building.h"

Building CreateBuilding(BuildingType type, int gridX, int gridY) {
    Building b;
    b.type = type;
    b.gridX = gridX;
    b.gridY = gridY;

    switch (type) {
        case BuildingType::RedHouse:
            b.width = 3;
            b.height = 3;
            // Texture is 48x58, footprint is 48x48 (3x3 tiles)
            // No horizontal overhang (48px = 3 tiles exactly)
            // Roof peak extends above: 58-48 = 10px offset up
            b.renderOffsetX = 0;
            b.renderOffsetY = -10;
            break;
        case BuildingType::House:
            b.width = 3;
            b.height = 3;
            // Texture is 48x64, footprint is 48x48 (3x3 tiles)
            // Roof extends above: 64-48 = 16px offset up
            b.renderOffsetX = 0;
            b.renderOffsetY = -16;
            break;
        case BuildingType::PizzaShop:
            b.width = 3;
            b.height = 3;
            // Texture is 48x48, footprint is 48x48 (3x3 tiles)
            b.renderOffsetX = 0;
            b.renderOffsetY = 0;
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
        case BuildingType::None:      return "None";
        case BuildingType::RedHouse:  return "Red House";
        case BuildingType::House:     return "House";
        case BuildingType::PizzaShop: return "Pizza Shop";
        default:                      return "Unknown";
    }
}

void BuildingTextures::Load() {
    textures[BuildingType::RedHouse] = LoadTexture("resources/redHouse.png");
    textures[BuildingType::House] = LoadTexture("resources/house.png");
    textures[BuildingType::PizzaShop] = LoadTexture("resources/pizzaShop.png");
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
