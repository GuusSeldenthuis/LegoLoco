#include "World.h"

World::World(int rows, int cols) : rows(rows), cols(cols) {
    tiles.resize(rows, std::vector<Tile>(cols));
}

void World::SetTile(int x, int y, TileType type, int orientation) {
    if (x >= 0 && x < cols && y >= 0 && y < rows) {
        tiles[y][x].type = type;
        tiles[y][x].orientation = orientation;
    }
}

Tile World::GetTile(int x, int y) const {
    if (x >= 0 && x < cols && y >= 0 && y < rows) {
        return tiles[y][x];
    }
    return Tile{};
}

void World::Render(GameCamera& camera, TileTextures& textures) {
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            Vector2 pos = WorldToScreen(x, y, camera.offset, camera.zoom);
            TileType type = tiles[y][x].type;

            if (type != TileType::Empty) {
                if (textures.HasTexture(type)) {
                    Texture2D tex = textures.Get(type);
                    Rectangle source = { 0, 0, (float)tex.width, (float)tex.height };
                    Rectangle dest = { pos.x, pos.y, tex.width * camera.zoom, tex.height * camera.zoom };
                    DrawTexturePro(tex, source, dest, {0, 0}, 0.0f, WHITE);
                } else {
                    float tileSize = TILE_SIZE * camera.zoom;
                    Color color = GetTileColor(type);
                    DrawRectangle((int)pos.x, (int)pos.y, (int)tileSize, (int)tileSize, color);
                }
            }
        }
    }
}
