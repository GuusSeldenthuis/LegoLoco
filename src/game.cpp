#include "raylib.h"
#include "Tile.h"
#include "Building.h"
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

    BuildingTextures buildingTextures;
    buildingTextures.Load();

    // Calculate world size based on background resolution
    int worldCols = screenWidth / TILE_SIZE;
    int worldRows = screenHeight / TILE_SIZE;
    World world(worldRows, worldCols);
    GameCamera camera;

    TileType selectedTile = TileType::Path;
    const TileType tileTypes[] = { TileType::Empty, TileType::Path, TileType::Track };
    const int tileTypeCount = 3;
    int selectedIndex = 1;

    // Building/placeable selection
    BuildingType selectedBuilding = BuildingType::None;
    bool buildingMode = false;

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
        if (IsKeyPressed(KEY_ONE))   { selectedIndex = 0; selectedTile = tileTypes[0]; buildingMode = false; }
        if (IsKeyPressed(KEY_TWO))   { selectedIndex = 1; selectedTile = tileTypes[1]; buildingMode = false; }
        if (IsKeyPressed(KEY_THREE)) { selectedIndex = 2; selectedTile = tileTypes[2]; buildingMode = false; }

        // Placeable selection
        if (IsKeyPressed(KEY_FOUR))  { selectedBuilding = BuildingType::Road; buildingMode = true; }
        if (IsKeyPressed(KEY_FIVE))  { selectedBuilding = BuildingType::RedHouse; buildingMode = true; }
        if (IsKeyPressed(KEY_SIX))   { selectedBuilding = BuildingType::House; buildingMode = true; }
        if (IsKeyPressed(KEY_SEVEN)) { selectedBuilding = BuildingType::PizzaShop; buildingMode = true; }

        // Get hovered tile
        Vector2 mousePos = GetMousePosition();
        Vector2 worldPos = ScreenToWorld(mousePos, camera.offset, camera.zoom);
        int hoverX = (int)floorf(worldPos.x);
        int hoverY = (int)floorf(worldPos.y);
        bool validHover = hoverX >= 0 && hoverX < world.GetCols() && hoverY >= 0 && hoverY < world.GetRows();

        // Place tile or building with left click
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && validHover) {
            if (buildingMode) {
                world.PlaceBuilding(selectedBuilding, hoverX, hoverY);
            } else {
                world.SetTile(hoverX, hoverY, selectedTile);
            }
        }

        // Continuous tile placement (drag) - only for tiles, not buildings
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && validHover && !buildingMode) {
            world.SetTile(hoverX, hoverY, selectedTile);
        }

        // Remove tile or building with right click
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && validHover) {
            if (buildingMode) {
                world.RemoveBuilding(hoverX, hoverY);
            } else {
                world.SetTile(hoverX, hoverY, TileType::Empty);
            }
        }

        // Continuous removal (drag) - only for tiles
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && !IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && validHover && !buildingMode) {
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
        world.RenderBuildings(camera, buildingTextures);

        // Draw hover highlight
        if (validHover) {
            float tileSize = TILE_SIZE * camera.zoom;
            Vector2 pos = WorldToScreen(hoverX, hoverY, camera.offset, camera.zoom);

            if (buildingMode && selectedBuilding != BuildingType::None) {
                // Get building dimensions for preview
                Building preview = CreateBuilding(selectedBuilding, hoverX, hoverY);
                float previewW = preview.width * tileSize;
                float previewH = preview.height * tileSize;

                // Check if placement is valid
                bool canPlace = world.CanPlace(preview);
                Color tint = canPlace ? Color{100, 255, 100, 150} : Color{255, 100, 100, 150};
                Color outlineColor = canPlace ? GREEN : RED;

                // Show texture preview with tint
                if (buildingTextures.HasTexture(selectedBuilding)) {
                    Texture2D tex = buildingTextures.Get(selectedBuilding);
                    Rectangle source = { 0, 0, (float)tex.width, (float)tex.height };
                    // Apply render offset to preview position
                    float previewX = pos.x + preview.renderOffsetX * camera.zoom;
                    float previewY = pos.y + preview.renderOffsetY * camera.zoom;
                    Rectangle dest = { previewX, previewY, tex.width * camera.zoom, tex.height * camera.zoom };
                    DrawTexturePro(tex, source, dest, {0, 0}, 0.0f, tint);
                }

                // Footprint outline
                DrawRectangleLines((int)pos.x, (int)pos.y, (int)previewW, (int)previewH, outlineColor);
            } else if (selectedTile != TileType::Empty) {
                // Show tile texture preview with green tint
                if (tileTextures.HasTexture(selectedTile)) {
                    Texture2D tex = tileTextures.Get(selectedTile);
                    Rectangle source = { 0, 0, (float)tex.width, (float)tex.height };
                    Rectangle dest = { pos.x, pos.y, tileSize, tileSize };
                    DrawTexturePro(tex, source, dest, {0, 0}, 0.0f, Color{100, 255, 100, 150});
                } else {
                    DrawRectangle((int)pos.x, (int)pos.y, (int)tileSize, (int)tileSize, Color{100, 255, 100, 100});
                }
                DrawRectangleLines((int)pos.x, (int)pos.y, (int)tileSize, (int)tileSize, GREEN);
            } else {
                // Empty/eraser - show red preview
                DrawRectangle((int)pos.x, (int)pos.y, (int)tileSize, (int)tileSize, Color{255, 100, 100, 100});
                DrawRectangleLines((int)pos.x, (int)pos.y, (int)tileSize, (int)tileSize, RED);
            }
        }

        // UI - Tile/Building palette
        DrawRectangle(10, 10, 160, 240, Color{0, 0, 0, 150});
        DrawText("Tiles (1-3):", 20, 20, 16, WHITE);
        for (int i = 0; i < tileTypeCount; i++) {
            int y = 45 + i * 18;
            Color textColor = (!buildingMode && i == selectedIndex) ? YELLOW : WHITE;
            const char* marker = (!buildingMode && i == selectedIndex) ? "> " : "  ";
            DrawText(TextFormat("%s%d: %s", marker, i + 1, GetTileName(tileTypes[i])), 20, y, 14, textColor);
        }

        const BuildingType buildingTypes[] = { BuildingType::Road, BuildingType::RedHouse, BuildingType::House, BuildingType::PizzaShop };
        DrawText("Placeables (4-7):", 20, 110, 16, WHITE);
        for (int i = 0; i < 4; i++) {
            int y = 135 + i * 18;
            Color textColor = (buildingMode && selectedBuilding == buildingTypes[i]) ? YELLOW : WHITE;
            const char* marker = (buildingMode && selectedBuilding == buildingTypes[i]) ? "> " : "  ";
            DrawText(TextFormat("%s%d: %s", marker, i + 4, GetBuildingName(buildingTypes[i])), 20, y, 14, textColor);
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
    buildingTextures.Unload();
    CloseWindow();
    return 0;
}
