#pragma once

struct Placeable {
    int gridX = 0;
    int gridY = 0;

    // Footprint size in tiles
    int width = 1;
    int height = 1;

    // Render offset to handle overhang (in pixels, before zoom)
    int renderOffsetX = 0;
    int renderOffsetY = 0;
};
