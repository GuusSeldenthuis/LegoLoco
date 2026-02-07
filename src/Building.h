#pragma once

#include "raylib.h"
#include "Placeable.h"
#include <unordered_map>

enum class BuildingType {
    None,
    Road,
    RedHouse,
    House,
    PizzaShop
};

struct Building : Placeable {
    BuildingType type = BuildingType::None;
};

Building CreateBuilding(BuildingType type, int gridX, int gridY);
const char* GetBuildingName(BuildingType type);

class BuildingTextures {
private:
    std::unordered_map<BuildingType, Texture2D> textures;
    bool loaded = false;

public:
    void Load();
    void Unload();
    bool HasTexture(BuildingType type) const;
    Texture2D Get(BuildingType type) const;
    bool IsLoaded() const { return loaded; }
};
