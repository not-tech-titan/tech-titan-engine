// editor.cpp
#include "raylib.h"
#include <vector>
#include <iostream>

// --- Constants ---
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 800;
const int GRID_SIZE = 50;
const float TILE_SIZE = 1.0f;

// --- Tile types ---
enum TileType { EMPTY, WALL, PLAYER };

// --- Tile struct ---
struct Tile {
    Vector3 position;
    TileType type = EMPTY;
};

// --- Toolbar tools ---
enum Tool { TOOL_WALL, TOOL_PLAYER };
Tool currentTool = TOOL_WALL;

// --- Grid ---
std::vector<Tile> grid(GRID_SIZE* GRID_SIZE);

// --- Camera ---
Camera camera;
bool camera3D = true;

// --- Initialization ---
void initGrid() {
    for (int z = 0; z < GRID_SIZE; z++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            grid[z * GRID_SIZE + x].position = { x * TILE_SIZE, 0.0f, z * TILE_SIZE };
            grid[z * GRID_SIZE + x].type = EMPTY;
        }
    }
}

void initCamera() {
    camera.position = { GRID_SIZE / 2.0f, 20.0f, GRID_SIZE / 2.0f };
    camera.target = { GRID_SIZE / 2.0f, 0.0f, GRID_SIZE / 2.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

// --- Input ---
void updateCamera(float dt) {
    const float speed = 10.0f;
    Vector3 forward = Vector3Subtract(camera.target, camera.position);
    forward = Vector3Normalize(forward);
    Vector3 right = Vector3CrossProduct(forward, camera.up);
    right = Vector3Normalize(right);

    if (IsKeyDown(KEY_W)) camera.position = Vector3Add(camera.position, Vector3Scale(forward, speed * dt));
    if (IsKeyDown(KEY_S)) camera.position = Vector3Subtract(camera.position, Vector3Scale(forward, speed * dt));
    if (IsKeyDown(KEY_D)) camera.position = Vector3Add(camera.position, Vector3Scale(right, speed * dt));
    if (IsKeyDown(KEY_A)) camera.position = Vector3Subtract(camera.position, Vector3Scale(right, speed * dt));

    camera.target = Vector3Add(camera.position, forward);

    if (IsKeyPressed(KEY_TAB)) {
        camera3D = !camera3D;
        camera.projection = camera3D ? CAMERA_PERSPECTIVE : CAMERA_ORTHOGRAPHIC;
        if (!camera3D) { // Top-down 2D view
            camera.position = { GRID_SIZE / 2.0f, 20.0f, GRID_SIZE / 2.0f };
            camera.target = { GRID_SIZE / 2.0f, 0.0f, GRID_SIZE / 2.0f };
        }
    }
}

// --- Toolbar (movable) ---
Vector2 toolbarPos = { 20, 20 };
Vector2 toolbarSize = { 120, 60 };
bool draggingToolbar = false;

void drawToolbar() {
    Vector2 mouse = GetMousePosition();
    Rectangle toolbarRect = { toolbarPos.x, toolbarPos.y, toolbarSize.x, toolbarSize.y };

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, toolbarRect)) {
        draggingToolbar = true;
    }
    if (IsMouseButtonUp(MOUSE_LEFT_BUTTON)) {
        draggingToolbar = false;
    }
    if (draggingToolbar) {
        toolbarPos = { mouse.x - toolbarSize.x / 2, mouse.y - toolbarSize.y / 2 };
    }

    DrawRectangleV(toolbarPos, toolbarSize, LIGHTGRAY);
    DrawText("Wall", toolbarPos.x + 10, toolbarPos.y + 5, 20, currentTool == TOOL_WALL ? GREEN : BLACK);
    DrawText("Player", toolbarPos.x + 10, toolbarPos.y + 30, 20, currentTool == TOOL_PLAYER ? BLUE : BLACK);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, toolbarRect)) {
        if (mouse.y - toolbarPos.y < 25) currentTool = TOOL_WALL;
        else currentTool = TOOL_PLAYER;
    }
}

// --- Grid interaction ---
void handleGridClick() {
    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return;
    Ray ray = GetMouseRay(GetMousePosition(), camera);

    for (auto& tile : grid) {
        BoundingBox box = { tile.position, { tile.position.x + TILE_SIZE, 0.1f, tile.position.z + TILE_SIZE } };
        if (CheckCollisionRayBox(ray, box)) {
            if (currentTool == TOOL_WALL) tile.type = WALL;
            else if (currentTool == TOOL_PLAYER) {
                // Only 1 player
                for (auto& t : grid) if (t.type == PLAYER) t.type = EMPTY;
                tile.type = PLAYER;
            }
        }
    }
}

// --- Drawing ---
void drawGrid() {
    for (auto& tile : grid) {
        Color c = WHITE;
        if (tile.type == WALL) c = RED;
        if (tile.type == PLAYER) c = BLUE;
        DrawCube(tile.position, TILE_SIZE, 0.1f, TILE_SIZE, c);
        DrawCubeWires(tile.position, TILE_SIZE, 0.1f, TILE_SIZE, DARKGRAY);
    }
}

// --- Main ---
int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Titan Editor");
    SetTargetFPS(60);

    initGrid();
    initCamera();

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        updateCamera(dt);
        handleGridClick();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        drawToolbar();

        BeginMode3D(camera);
        drawGrid();
        EndMode3D();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
