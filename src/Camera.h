#pragma once

#include "raylib.h"
#include "Tile.h"

Vector2 WorldToScreen(int gridX, int gridY, Vector2 cameraOffset, float zoom);
Vector2 ScreenToWorld(Vector2 screenPos, Vector2 cameraOffset, float zoom);

class GameCamera {
public:
    Vector2 offset;
    float zoom;
    float minZoom = 0.25f;
    float maxZoom = 3.0f;

    GameCamera(float startX, float startY) : offset{startX, startY}, zoom(1.0f) {}

    void Update() {
        // Pan with middle mouse drag
        if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
            Vector2 delta = GetMouseDelta();
            offset.x += delta.x;
            offset.y += delta.y;
        }

        // Zoom with mouse wheel
        float wheel = GetMouseWheelMove();
        if (wheel != 0) {
            Vector2 mousePos = GetMousePosition();
            Vector2 worldBeforeZoom = ScreenToWorld(mousePos, offset, zoom);

            zoom += wheel * 0.1f;
            if (zoom < minZoom) zoom = minZoom;
            if (zoom > maxZoom) zoom = maxZoom;

            Vector2 screenAfterZoom = WorldToScreen((int)worldBeforeZoom.x, (int)worldBeforeZoom.y, offset, zoom);
            offset.x += mousePos.x - screenAfterZoom.x;
            offset.y += mousePos.y - screenAfterZoom.y;
        }
    }
};
