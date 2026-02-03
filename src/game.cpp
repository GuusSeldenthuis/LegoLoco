#include "raylib.h"

class Grid {
private:
    int rows;
    int cols;
    int cellSize;
    Color lineColor;

public:
    Grid(int rows, int cols, int cellSize) 
        : rows(rows), cols(cols), cellSize(cellSize), lineColor(DARKGRAY) {}

    void Render() {
        // Draw vertical lines
        for (int x = 0; x <= cols; x++) {
            DrawLine(x * cellSize, 0, x * cellSize, rows * cellSize, lineColor);
        }
        
        // Draw horizontal lines
        for (int y = 0; y <= rows; y++) {
            DrawLine(0, y * cellSize, cols * cellSize, y * cellSize, lineColor);
        }
    }

    int GetCellSize() const { return cellSize; }
    int GetRows() const { return rows; }
    int GetCols() const { return cols; }
};

// Main.cpp
int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    
    InitWindow(screenWidth, screenHeight, "Lego Loco - Grid");
    SetTargetFPS(60);
    
    Grid grid(20, 25, 32); // 20 rows, 25 cols, 32px cells
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        grid.Render();
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
