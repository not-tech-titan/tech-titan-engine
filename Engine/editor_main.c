#include "raylib.h"
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define GRID_SIZE 32
#define MAX_BLOCKS 1024
#define TOOLBAR_HEIGHT 48

typedef enum {
    TOOL_BLOCK,
    TOOL_SLOPE,
    TOOL_PLAYER
} ToolType;

typedef enum {
    BLOCK_RECT,
    BLOCK_SLOPE_LU, // slope created by dragging left+up (type identifier only)
    BLOCK_SLOPE_RU, // drag right+up
    BLOCK_SLOPE_LD, // drag left+down
    BLOCK_SLOPE_RD  // drag right+down
} BlockType;

typedef struct {
    Vector2 position; // top-left of bounding rect
    Vector2 size;     // width/height of bounding rect
    BlockType type;
    bool active;
} LevelBlock;

typedef struct {
    Vector2 position;
    bool placed;
} PlayerMarker;

static LevelBlock blocks[MAX_BLOCKS];
static int blockCount = 0;
static PlayerMarker player = { {0,0}, false };

// temp vars for drawing in-progress block
static bool isDrawingBlock = false;
static Vector2 blockStart = { 0, 0 };

// Helper: compute triangle vertices and type from bounding rect + drag signs
static void ComputeSlopeVerticesFromBounds(BlockType type, Vector2 pos, Vector2 size,
    Vector2* outV1, Vector2* outV2, Vector2* outV3)
{
    // corners
    Vector2 TL = pos;
    Vector2 TR = (Vector2){ pos.x + size.x, pos.y };
    Vector2 BL = (Vector2){ pos.x, pos.y + size.y };
    Vector2 BR = (Vector2){ pos.x + size.x, pos.y + size.y };

    switch (type) {
    case BLOCK_SLOPE_RD: // dragged right+down -> triangle TL, TR, BR
        *outV1 = TL; *outV2 = TR; *outV3 = BR;
        break;
    case BLOCK_SLOPE_LD: // dragged left+down -> triangle TL, BL, BR
        *outV1 = TL; *outV2 = BL; *outV3 = BR;
        break;
    case BLOCK_SLOPE_RU: // dragged right+up -> triangle BL, TL, TR
        *outV1 = BL; *outV2 = TL; *outV3 = TR;
        break;
    case BLOCK_SLOPE_LU: // dragged left+up -> triangle BR, BL, TR
        *outV1 = BR; *outV2 = BL; *outV3 = TR;
        break;
    default:
        *outV1 = TL; *outV2 = TR; *outV3 = BR;
        break;
    }
}

static void DrawSlope(BlockType type, Vector2 pos, Vector2 size, Color color)
{
    Vector2 v1, v2, v3;
    ComputeSlopeVerticesFromBounds(type, pos, size, &v1, &v2, &v3);
    DrawTriangle(v1, v2, v3, color);
}

static void DrawSlopeOutline(BlockType type, Vector2 pos, Vector2 size, Color color)
{
    Vector2 v1, v2, v3;
    ComputeSlopeVerticesFromBounds(type, pos, size, &v1, &v2, &v3);
    DrawTriangleLines(v1, v2, v3, color);
}

// ---------------- SAVE / LOAD ----------------
void SaveLevel(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) return;

    if (player.placed) {
        fprintf(file, "PLAYER %.0f %.0f\n", player.position.x, player.position.y);
    }

    for (int i = 0; i < blockCount; i++) {
        if (blocks[i].active) {
            // Save: type x y w h
            fprintf(file, "BLOCK %d %.0f %.0f %.0f %.0f\n",
                (int)blocks[i].type,
                blocks[i].position.x, blocks[i].position.y,
                blocks[i].size.x, blocks[i].size.y);
        }
    }

    fclose(file);
    printf("Saved level to %s\n", filename);
}

void LoadLevel(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return;

    blockCount = 0;
    player.placed = false;

    char tok[32];
    while (fscanf(file, "%31s", tok) == 1) {
        if (strcmp(tok, "PLAYER") == 0) {
            float px, py;
            if (fscanf(file, "%f %f", &px, &py) == 2) {
                player.position = (Vector2){ px, py };
                player.placed = true;
            }
        }
        else if (strcmp(tok, "BLOCK") == 0 && blockCount < MAX_BLOCKS) {
            int btype;
            float x, y, w, h;
            if (fscanf(file, "%d %f %f %f %f", &btype, &x, &y, &w, &h) == 5) {
                blocks[blockCount].type = (BlockType)btype;
                blocks[blockCount].position = (Vector2){ x, y };
                blocks[blockCount].size = (Vector2){ w, h };
                blocks[blockCount].active = true;
                blockCount++;
            }
            else {
                // if old format (no type present) try to recover by reading 4 floats only
                rewind(file);
                break;
            }
        }
        else {
            // unknown token, try to skip line
            char buffer[256];
            fgets(buffer, sizeof(buffer), file);
        }
    }

    fclose(file);
    printf("Loaded level from %s\n", filename);
}

// ------------------------------------------------

int main(void) {
    InitWindow(1000, 700, "TechTitan Editor");
    SetTargetFPS(60);

    Camera2D camera = { 0 };
    camera.target = (Vector2){ 0, 0 };
    camera.offset = (Vector2){ GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
    camera.zoom = 1.0f;

    ToolType currentTool = TOOL_BLOCK;

    while (!WindowShouldClose()) {
        // --- Input ---
        float moveSpeed = 10.0f / camera.zoom;
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) camera.target.y -= moveSpeed;
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) camera.target.y += moveSpeed;
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) camera.target.x -= moveSpeed;
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) camera.target.x += moveSpeed;

        camera.zoom += GetMouseWheelMove() * 0.1f;
        if (camera.zoom < 0.2f) camera.zoom = 0.2f;

        if (IsKeyPressed(KEY_ONE)) currentTool = TOOL_BLOCK;
        if (IsKeyPressed(KEY_TWO)) currentTool = TOOL_PLAYER;
        if (IsKeyPressed(KEY_THREE)) currentTool = TOOL_SLOPE;

        Vector2 mouseScreen = GetMousePosition();
        Vector2 mouseWorld = GetScreenToWorld2D(mouseScreen, camera);
        Vector2 gridPos = {
            floorf(mouseWorld.x / GRID_SIZE) * GRID_SIZE,
            floorf(mouseWorld.y / GRID_SIZE) * GRID_SIZE
        };

        bool mouseOverToolbar = (GetMouseY() < TOOLBAR_HEIGHT);

        // ---- Placement ----
        if (!mouseOverToolbar) {
            if (currentTool == TOOL_BLOCK || currentTool == TOOL_SLOPE) {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    blockStart = gridPos;
                    isDrawingBlock = true;
                }

                if (isDrawingBlock && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                    if (blockCount < MAX_BLOCKS) {
                        Vector2 end = gridPos;

                        // compute bounding rect from start and end
                        float x = fminf(blockStart.x, end.x);
                        float y = fminf(blockStart.y, end.y);
                        float w = fabsf(blockStart.x - end.x) + GRID_SIZE;
                        float h = fabsf(blockStart.y - end.y) + GRID_SIZE;

                        blocks[blockCount].position = (Vector2){ x, y };
                        blocks[blockCount].size = (Vector2){ w, h };
                        blocks[blockCount].active = true;

                        if (currentTool == TOOL_BLOCK) {
                            blocks[blockCount].type = BLOCK_RECT;
                        }
                        else {
                            // determine drag directions by comparing end vs start
                            float dx = end.x - blockStart.x;
                            float dy = end.y - blockStart.y;
                            if (dx >= 0 && dy >= 0) blocks[blockCount].type = BLOCK_SLOPE_RD; // right + down
                            else if (dx < 0 && dy >= 0) blocks[blockCount].type = BLOCK_SLOPE_LD; // left + down
                            else if (dx >= 0 && dy < 0) blocks[blockCount].type = BLOCK_SLOPE_RU; // right + up
                            else blocks[blockCount].type = BLOCK_SLOPE_LU; // left + up
                        }

                        blockCount++;
                    }
                    isDrawingBlock = false;
                }

                if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                    // delete block under mouse (using bounding rect)
                    for (int i = 0; i < blockCount; i++) {
                        if (!blocks[i].active) continue;
                        Rectangle r = { blocks[i].position.x, blocks[i].position.y,
                                        blocks[i].size.x, blocks[i].size.y };
                        if (CheckCollisionPointRec(mouseWorld, r)) {
                            blocks[i].active = false;
                        }
                    }
                }
            }
            else if (currentTool == TOOL_PLAYER) {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    player.position = gridPos;
                    player.placed = true;
                }
            }
        }

        // Save / Load hotkeys
        if (IsKeyPressed(KEY_F5)) SaveLevel("level.txt");
        if (IsKeyPressed(KEY_F9)) LoadLevel("level.txt");

        // --- Drawing ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode2D(camera);

        // draw grid (account for top toolbar offset in world)
        Vector2 topLeft = GetScreenToWorld2D((Vector2) { 0, TOOLBAR_HEIGHT }, camera);
        Vector2 bottomRight = GetScreenToWorld2D((Vector2) { GetScreenWidth(), GetScreenHeight() }, camera);

        int startX = (int)floorf(topLeft.x / GRID_SIZE) * GRID_SIZE;
        int endX = (int)floorf(bottomRight.x / GRID_SIZE) * GRID_SIZE;
        int startY = (int)floorf(topLeft.y / GRID_SIZE) * GRID_SIZE;
        int endY = (int)floorf(bottomRight.y / GRID_SIZE) * GRID_SIZE;

        for (int x = startX; x <= endX; x += GRID_SIZE) DrawLine(x, startY, x, endY, LIGHTGRAY);
        for (int y = startY; y <= endY; y += GRID_SIZE) DrawLine(startX, y, endX, y, LIGHTGRAY);

        // draw blocks
        for (int i = 0; i < blockCount; i++) {
            if (!blocks[i].active) continue;
            if (blocks[i].type == BLOCK_RECT) {
                DrawRectangleV(blocks[i].position, blocks[i].size, DARKGRAY);
            }
            else {
                DrawSlope(blocks[i].type, blocks[i].position, blocks[i].size, DARKGRAY);
                // optional outline for clarity:
                Vector2 v1, v2, v3;
                ComputeSlopeVerticesFromBounds(blocks[i].type, blocks[i].position, blocks[i].size, &v1, &v2, &v3);
                DrawTriangleLines(v1, v2, v3, BLACK);
            }
        }

        // player marker
        if (player.placed) {
            DrawRectangle(player.position.x, player.position.y, GRID_SIZE, GRID_SIZE, BLUE);
        }

        // preview while dragging
        if (!mouseOverToolbar && isDrawingBlock) {
            Vector2 end = gridPos;
            float x = fminf(blockStart.x, end.x);
            float y = fminf(blockStart.y, end.y);
            float w = fabsf(blockStart.x - end.x) + GRID_SIZE;
            float h = fabsf(blockStart.y - end.y) + GRID_SIZE;

            if (currentTool == TOOL_BLOCK) {
                DrawRectangleLines(x, y, w, h, RED);
            }
            else { // slope preview
                float dx = end.x - blockStart.x;
                float dy = end.y - blockStart.y;
                BlockType previewType;
                if (dx >= 0 && dy >= 0) previewType = BLOCK_SLOPE_RD;
                else if (dx < 0 && dy >= 0) previewType = BLOCK_SLOPE_LD;
                else if (dx >= 0 && dy < 0) previewType = BLOCK_SLOPE_RU;
                else previewType = BLOCK_SLOPE_LU;
                DrawSlopeOutline(previewType, (Vector2) { x, y }, (Vector2) { w, h }, RED);
            }
        }
        else if (!mouseOverToolbar) {
            // simple hover cell preview
            DrawRectangleLines(gridPos.x, gridPos.y, GRID_SIZE, GRID_SIZE, RED);
        }

        EndMode2D();

        // --- Toolbar UI ---
        DrawRectangle(0, 0, GetScreenWidth(), TOOLBAR_HEIGHT, GRAY);
        DrawText("Toolbar:", 10, 12, 20, BLACK);

        Rectangle blockBtn = { 120, 8, 80, 32 };
        DrawRectangleRec(blockBtn, currentTool == TOOL_BLOCK ? DARKGRAY : LIGHTGRAY);
        DrawText("Block", blockBtn.x + 10, blockBtn.y + 8, 16, BLACK);

        Rectangle playerBtn = { 220, 8, 80, 32 };
        DrawRectangleRec(playerBtn, currentTool == TOOL_PLAYER ? DARKGRAY : LIGHTGRAY);
        DrawText("Player", playerBtn.x + 10, playerBtn.y + 8, 16, BLACK);

        Rectangle slopeBtn = { 320, 8, 80, 32 };
        DrawRectangleRec(slopeBtn, currentTool == TOOL_SLOPE ? DARKGRAY : LIGHTGRAY);
        DrawText("Slope", slopeBtn.x + 10, slopeBtn.y + 8, 16, BLACK);

        // toolbar click handling
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && GetMouseY() < TOOLBAR_HEIGHT) {
            Vector2 ms = GetMousePosition();
            if (CheckCollisionPointRec(ms, blockBtn)) currentTool = TOOL_BLOCK;
            else if (CheckCollisionPointRec(ms, playerBtn)) currentTool = TOOL_PLAYER;
            else if (CheckCollisionPointRec(ms, slopeBtn)) currentTool = TOOL_SLOPE;
        }

        DrawText("F5: Save | F9: Load | 1: Block | 2: Player | 3: Slope", 10, TOOLBAR_HEIGHT + 10, 20, BLACK);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
