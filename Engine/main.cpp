#include "raylib.h"
#include <iostream>

// Optional: define your own Update and Draw functions for clarity
void Update()
{
    // Handle input or update game logic here
    if (IsKeyPressed(KEY_SPACE))
    {
        std::cout << "Space pressed!" << std::endl;
    }
}

void Draw()
{
    // Draw your game objects here
    DrawText("This will soon become the best game engine in history!!!!", 20, 20, 20, DARKGRAY);
}

int main()
{
    // Window setup
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Titan Engine");

    SetTargetFPS(60); // Set desired FPS

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update
        Update();

        // Start drawing
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw
        Draw();

        // End drawing
        EndDrawing();
    }

    // Cleanup
    CloseWindow();

    return 0;
}
