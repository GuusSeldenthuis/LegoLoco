#include "raylib.h"
#include "Tile.h"
#include "Building.h"
#include "Camera.h"
#include "World.h"
#include "SaveFileHandler.h"
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

    // Load toybox texture (first frame from sprite sheet, remove magenta background)
    Image toyboxImage = LoadImage("resources/raw/toybox.bmp");
    ImageCrop(&toyboxImage, {53, 50, 61, 55});
    ImageColorReplace(&toyboxImage, {255, 0, 255, 255}, {0, 0, 0, 0});
    ImageFormat(&toyboxImage, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    Texture2D toyboxTexture = LoadTextureFromImage(toyboxImage);
    UnloadImage(toyboxImage);

    // Toybox drag state
    Vector2 toyboxPos = {(float)(screenWidth - toyboxTexture.width - 20), 20.0f};
    bool toyboxDragging = false;
    Vector2 toyboxDragOffset = {0, 0};

    // Calculate world size based on background resolution
    int worldCols = screenWidth / TILE_SIZE;
    int worldRows = screenHeight / TILE_SIZE;
    World world(worldRows, worldCols);
    SaveFileHandler saveHandler;
    GameCamera camera;

    TileType selectedTile = TileType::Path;
    const TileType tileTypes[] = { TileType::Empty, TileType::Path, TileType::Road, TileType::Track, TileType::TrackCorner };
    const int tileTypeCount = 5;
    int selectedIndex = 1;
    float previewRotation = 0.0f;

    // Building/placeable selection
    BuildingType selectedBuilding = BuildingType::None;
    bool buildingMode = false;

    // Debug
    bool showDebug = false;

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
            if (saveHandler.Save(world, SAVE_PATH)) {
                statusMessage = "World saved!";
            } else {
                statusMessage = "Failed to save!";
            }
            statusTimer = 2.0f;
        }

        // Load with Ctrl+L
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_L)) {
            if (saveHandler.Load(world, SAVE_PATH)) {
                statusMessage = "World loaded!";
                world.RebuildPathGraph();
            } else {
                statusMessage = "Failed to load!";
            }
            statusTimer = 2.0f;
        }

        // Tile selection with number keys
        if (IsKeyPressed(KEY_ONE))   { selectedIndex = 0; selectedTile = tileTypes[0]; buildingMode = false; previewRotation = 0.0f; }
        if (IsKeyPressed(KEY_TWO))   { selectedIndex = 1; selectedTile = tileTypes[1]; buildingMode = false; previewRotation = 0.0f; }
        if (IsKeyPressed(KEY_THREE)) { selectedIndex = 2; selectedTile = tileTypes[2]; buildingMode = false; previewRotation = 0.0f; }
        if (IsKeyPressed(KEY_FOUR))  { selectedIndex = 3; selectedTile = tileTypes[3]; buildingMode = false; previewRotation = 0.0f; }
        if (IsKeyPressed(KEY_FIVE))  { selectedIndex = 4; selectedTile = tileTypes[4]; buildingMode = false; previewRotation = 0.0f; }

        // Placeable selection
        if (IsKeyPressed(KEY_SIX))   { selectedBuilding = BuildingType::RedHouse; buildingMode = true; }
        if (IsKeyPressed(KEY_SEVEN)) { selectedBuilding = BuildingType::House; buildingMode = true; }
        if (IsKeyPressed(KEY_EIGHT)) { selectedBuilding = BuildingType::PizzaShop; buildingMode = true; }

        // Debug toggle
        if (IsKeyPressed(KEY_F1)) showDebug = !showDebug;

        // Toybox dragging
        Vector2 mousePos = GetMousePosition();
        Rectangle toyboxRect = {toyboxPos.x, toyboxPos.y, (float)toyboxTexture.width, (float)toyboxTexture.height};
        bool mouseOverToybox = CheckCollisionPointRec(mousePos, toyboxRect);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mouseOverToybox) {
            toyboxDragging = true;
            toyboxDragOffset = {mousePos.x - toyboxPos.x, mousePos.y - toyboxPos.y};
        }
        if (toyboxDragging) {
            toyboxPos = {mousePos.x - toyboxDragOffset.x, mousePos.y - toyboxDragOffset.y};
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                toyboxDragging = false;
            }
        }

        // Get hovered tile
        Vector2 worldPos = ScreenToWorld(mousePos, camera.offset, camera.zoom);
        int hoverX = (int)floorf(worldPos.x);
        int hoverY = (int)floorf(worldPos.y);
        bool validHover = hoverX >= 0 && hoverX < world.GetCols() && hoverY >= 0 && hoverY < world.GetRows();

        bool tilesChanged = false;
        bool isTrackType = (selectedTile == TileType::Track || selectedTile == TileType::TrackCorner);

        // Place tile or building with left click (not when dragging toybox)
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && validHover && !toyboxDragging && !mouseOverToybox) {
            if (buildingMode) {
                world.PlaceBuilding(selectedBuilding, hoverX, hoverY);
            } else {
                world.SetTile(hoverX, hoverY, selectedTile, previewRotation);
                tilesChanged = true;
            }
        }

        // Continuous tile placement (drag) - only for non-track 1x1 tiles
        bool is1x1Tile = (GetTileWidth(selectedTile) == 1 && GetTileHeight(selectedTile) == 1);
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && validHover && !buildingMode && is1x1Tile && !isTrackType && !toyboxDragging) {
            world.SetTile(hoverX, hoverY, selectedTile, previewRotation);
            tilesChanged = true;
        }

        // Right click: rotate preview for tracks, remove for everything else
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            if (!buildingMode && isTrackType) {
                previewRotation = fmodf(previewRotation + 90.0f, 360.0f);
            } else if (validHover) {
                if (buildingMode) {
                    world.RemoveBuilding(hoverX, hoverY);
                } else {
                    world.SetTile(hoverX, hoverY, TileType::Empty);
                    tilesChanged = true;
                }
            }
        }

        // Continuous removal (drag) - only for non-track tiles
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && !IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && validHover && !buildingMode && !isTrackType) {
            world.SetTile(hoverX, hoverY, TileType::Empty);
            tilesChanged = true;
        }

        if (tilesChanged) world.RebuildPathGraph();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw background (scales and pans with camera)
        Rectangle bgSource = { 0, 0, (float)background.width, (float)background.height };
        Rectangle bgDest = { camera.offset.x, camera.offset.y,
                             background.width * camera.zoom, background.height * camera.zoom };
        DrawTexturePro(background, bgSource, bgDest, {0, 0}, 0.0f, WHITE);

        world.Render(camera, tileTextures);
        world.RenderBuildings(camera, buildingTextures);

        if (showDebug) world.RenderPathDebug(camera);

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
                // Get tile dimensions (handles multi-tile like 2x2 roads)
                int tileW = GetTileWidth(selectedTile);
                int tileH = GetTileHeight(selectedTile);
                float previewW = tileSize * tileW;
                float previewH = tileSize * tileH;

                // Check if placement is valid (within bounds)
                bool canPlace = (hoverX + tileW <= world.GetCols() && hoverY + tileH <= world.GetRows());
                Color tint = canPlace ? Color{100, 255, 100, 150} : Color{255, 100, 100, 150};
                Color outlineColor = canPlace ? GREEN : RED;

                // Determine preview shape based on tile type
                TileShape previewShape;
                if (selectedTile == TileType::Path) previewShape = TileShape::Single;
                else if (selectedTile == TileType::TrackCorner) previewShape = TileShape::Corner;
                else previewShape = TileShape::Straight;

                if (tileTextures.HasTexture(selectedTile, previewShape)) {
                    Texture2D tex = tileTextures.Get(selectedTile, previewShape);
                    Rectangle source = { 0, 0, (float)tex.width, (float)tex.height };
                    float centerX = pos.x + previewW / 2;
                    float centerY = pos.y + previewH / 2;
                    Rectangle dest = { centerX, centerY, previewW, previewH };
                    Vector2 origin = { previewW / 2, previewH / 2 };
                    DrawTexturePro(tex, source, dest, origin, previewRotation, tint);
                } else {
                    DrawRectangle((int)pos.x, (int)pos.y, (int)previewW, (int)previewH, tint);
                }
                DrawRectangleLines((int)pos.x, (int)pos.y, (int)previewW, (int)previewH, outlineColor);
            } else {
                // Empty/eraser - show red preview
                DrawRectangle((int)pos.x, (int)pos.y, (int)tileSize, (int)tileSize, Color{255, 100, 100, 100});
                DrawRectangleLines((int)pos.x, (int)pos.y, (int)tileSize, (int)tileSize, RED);
            }
        }

        // Draw toybox
        DrawTexture(toyboxTexture, (int)toyboxPos.x, (int)toyboxPos.y, WHITE);

        // UI - Tile/Building palette
        DrawRectangle(10, 10, 180, 240, Color{0, 0, 0, 150});
        DrawText("Tiles (1-5):", 20, 20, 16, WHITE);
        for (int i = 0; i < tileTypeCount; i++) {
            int y = 45 + i * 18;
            Color textColor = (!buildingMode && i == selectedIndex) ? YELLOW : WHITE;
            const char* marker = (!buildingMode && i == selectedIndex) ? "> " : "  ";
            DrawText(TextFormat("%s%d: %s", marker, i + 1, GetTileName(tileTypes[i])), 20, y, 14, textColor);
        }

        const BuildingType buildingTypes[] = { BuildingType::RedHouse, BuildingType::House, BuildingType::PizzaShop };
        DrawText("Placeables (6-8):", 20, 143, 16, WHITE);
        for (int i = 0; i < 3; i++) {
            int y = 168 + i * 18;
            Color textColor = (buildingMode && selectedBuilding == buildingTypes[i]) ? YELLOW : WHITE;
            const char* marker = (buildingMode && selectedBuilding == buildingTypes[i]) ? "> " : "  ";
            DrawText(TextFormat("%s%d: %s", marker, i + 6, GetBuildingName(buildingTypes[i])), 20, y, 14, textColor);
        }

        // Debug info
        DrawRectangle(5, screenHeight - 55, screenWidth - 10, 50, Color{0, 0, 0, 150});
        DrawText(TextFormat("Grid: %d, %d", hoverX, hoverY), 10, screenHeight - 50, 16, WHITE);
        if (!buildingMode && isTrackType) {
            DrawText(TextFormat("LMB: Place | RMB: Rotate (%d) | MMB: Pan | Scroll: Zoom | Ctrl+S/L: Save/Load | F1: Debug", (int)previewRotation), 10, screenHeight - 25, 14, LIGHTGRAY);
        } else {
            DrawText("LMB: Place | RMB: Remove | MMB: Pan | Scroll: Zoom | Ctrl+S/L: Save/Load | F1: Debug", 10, screenHeight - 25, 14, LIGHTGRAY);
        }

        // Status message
        if (!statusMessage.empty()) {
            int textWidth = MeasureText(statusMessage.c_str(), 20);
            DrawRectangle(screenWidth/2 - textWidth/2 - 10, 10, textWidth + 20, 30, Color{0, 0, 0, 180});
            DrawText(statusMessage.c_str(), screenWidth/2 - textWidth/2, 15, 20, GREEN);
        }

        EndDrawing();
    }

    UnloadTexture(toyboxTexture);
    UnloadTexture(background);
    tileTextures.Unload();
    buildingTextures.Unload();
    CloseWindow();
    return 0;
}
