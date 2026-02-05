#pragma once

#include "raylib.h"
#include <unordered_map>

enum class BuildingType {
    None,
    RedHouse
};

struct Building {
    BuildingType type = BuildingType::None;
    int gridX = 0;
    int gridY = 0;

    // Footprint size in tiles
    int width = 2;
    int height = 2;

    // Render offset to handle roof/overhang (in pixels, before zoom)
    // Negative values shift the sprite up/left
    int renderOffsetX = 0;
    int renderOffsetY = 0;
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
