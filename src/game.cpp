#include "raylib.h"
#include "Tile.h"
#include "Camera.h"
#include "World.h"
#include <cmath>
#include <string>

const char* SAVE_PATH = "saves/world.json";

Vector2 WorldToScreen(int gridX, int gridY, Vector2 cameraOffset, float zoom) {
    float screenX = gridX * TILE_SIZE * zoom + cameraOffset.x;
    float screenY = gridY * TILE_SIZE * zoom + cameraOffset.y;
    return { screenX, screenY };
}

Vector2 ScreenToWorld(Vector2 screenPos, Vector2 cameraOffset, float zoom) {
    float gridX = (screenPos.x - cameraOffset.x) / (TILE_SIZE * zoom);
    float gridY = (screenPos.y - cameraOffset.y) / (TILE_SIZE * zoom);
    return { gridX, gridY };
}

int main() {
    // Load background to get its dimensions
    Image bgImage = LoadImage("resources/background00.png");
    const int screenWidth = bgImage.width;
    const int screenHeight = bgImage.height;

    InitWindow(screenWidth, screenHeight, "openLegoLoco");
    SetTargetFPS(60);

    // Convert image to texture and unload image
    Texture2D background = LoadTextureFromImage(bgImage);
    UnloadImage(bgImage);

    TileTextures tileTextures;
    tileTextures.Load();

    // Calculate world size based on background resolution
    int worldCols = screenWidth / TILE_SIZE;
    int worldRows = screenHeight / TILE_SIZE;
    World world(worldRows, worldCols);
    GameCamera camera;

    TileType selectedTile = TileType::Grass;
    const TileType tileTypes[] = { TileType::Empty, TileType::Grass, TileType::Road, TileType::Water, TileType::Track };
    const int tileTypeCount = 5;
    int selectedIndex = 1;

    // Status message
    std::string statusMessage = "";
    float statusTimer = 0.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        camera.Update();

        // Update status timer
        if (statusTimer > 0) {
            statusTimer -= dt;
            if (statusTimer <= 0) statusMessage = "";
        }

        // Save with Ctrl+S
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S)) {
            if (world.Save(SAVE_PATH)) {
                statusMessage = "World saved!";
            } else {
                statusMessage = "Failed to save!";
            }
            statusTimer = 2.0f;
        }

        // Load with Ctrl+L
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_L)) {
            if (world.Load(SAVE_PATH)) {
                statusMessage = "World loaded!";
            } else {
                statusMessage = "Failed to load!";
            }
            statusTimer = 2.0f;
        }

        // Tile selection with number keys
        if (IsKeyPressed(KEY_ONE))   { selectedIndex = 0; selectedTile = tileTypes[0]; }
        if (IsKeyPressed(KEY_TWO))   { selectedIndex = 1; selectedTile = tileTypes[1]; }
        if (IsKeyPressed(KEY_THREE)) { selectedIndex = 2; selectedTile = tileTypes[2]; }
        if (IsKeyPressed(KEY_FOUR))  { selectedIndex = 3; selectedTile = tileTypes[3]; }
        if (IsKeyPressed(KEY_FIVE))  { selectedIndex = 4; selectedTile = tileTypes[4]; }

        // Get hovered tile
        Vector2 mousePos = GetMousePosition();
        Vector2 worldPos = ScreenToWorld(mousePos, camera.offset, camera.zoom);
        int hoverX = (int)floorf(worldPos.x);
        int hoverY = (int)floorf(worldPos.y);
        bool validHover = hoverX >= 0 && hoverX < world.GetCols() && hoverY >= 0 && hoverY < world.GetRows();

        // Place tile with left click
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && validHover) {
            world.SetTile(hoverX, hoverY, selectedTile);
        }

        // Remove tile with right click
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && validHover) {
            world.SetTile(hoverX, hoverY, TileType::Empty);
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw background (scales and pans with camera)
        Rectangle bgSource = { 0, 0, (float)background.width, (float)background.height };
        Rectangle bgDest = { camera.offset.x, camera.offset.y,
                             background.width * camera.zoom, background.height * camera.zoom };
        DrawTexturePro(background, bgSource, bgDest, {0, 0}, 0.0f, WHITE);

        world.Render(camera, tileTextures);

        // Draw hover highlight
        if (validHover) {
            float tileSize = TILE_SIZE * camera.zoom;
            Vector2 pos = WorldToScreen(hoverX, hoverY, camera.offset, camera.zoom);
            DrawRectangle((int)pos.x, (int)pos.y, (int)tileSize, (int)tileSize, Color{255, 255, 255, 100});
            DrawRectangleLines((int)pos.x, (int)pos.y, (int)tileSize, (int)tileSize, WHITE);
        }

        // UI - Tile palette
        DrawRectangle(10, 10, 150, 130, Color{0, 0, 0, 150});
        DrawText("Tiles (1-5):", 20, 20, 16, WHITE);
        for (int i = 0; i < tileTypeCount; i++) {
            int y = 45 + i * 18;
            Color textColor = (i == selectedIndex) ? YELLOW : WHITE;
            const char* marker = (i == selectedIndex) ? "> " : "  ";
            DrawText(TextFormat("%s%d: %s", marker, i + 1, GetTileName(tileTypes[i])), 20, y, 14, textColor);
        }

        // Debug info
        DrawRectangle(5, screenHeight - 55, screenWidth - 10, 50, Color{0, 0, 0, 150});
        DrawText(TextFormat("Grid: %d, %d", hoverX, hoverY), 10, screenHeight - 50, 16, WHITE);
        DrawText("LMB: Place | RMB: Remove | MMB: Pan | Scroll: Zoom | Ctrl+S: Save | Ctrl+L: Load", 10, screenHeight - 25, 14, LIGHTGRAY);

        // Status message
        if (!statusMessage.empty()) {
            int textWidth = MeasureText(statusMessage.c_str(), 20);
            DrawRectangle(screenWidth/2 - textWidth/2 - 10, 10, textWidth + 20, 30, Color{0, 0, 0, 180});
            DrawText(statusMessage.c_str(), screenWidth/2 - textWidth/2, 15, 20, GREEN);
        }

        EndDrawing();
    }

    UnloadTexture(background);
    tileTextures.Unload();
    CloseWindow();
    return 0;
}
