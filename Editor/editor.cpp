#include "raylib.h"
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>

//--------------------------------------------------
// Config
//--------------------------------------------------
constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;
constexpr int GRID_SIZE = 20;

constexpr float TITLE_BAR_HEIGHT = 26.0f;
constexpr float TOOLBAR_HEIGHT = 30.0f;
constexpr float DRAG_HANDLE_WIDTH = 50.0f;

constexpr float MIN_WINDOW_W = 180.0f;
constexpr float MIN_WINDOW_H = 140.0f;

constexpr float UI_SNAP = 16.0f;

//--------------------------------------------------
// Types
//--------------------------------------------------
enum TileType { EMPTY, WALL, PLAYER, EXIT, ERASER };
enum AxisMode { AXIS_XY, AXIS_XZ };

struct Tile {
    Vector2 pos{};
    TileType type = EMPTY;
};

struct Window {
    Vector2 pos{};
    Vector2 size{};
    std::string title;
    bool open = true;
    bool dragging = false;
    bool resizing = false;
    int zOrder = 0;
};

//--------------------------------------------------
// Globals
//--------------------------------------------------
std::vector<Tile> grid(GRID_SIZE* GRID_SIZE);
TileType currentTool = WALL;

bool showGrid = true;
bool preview2D = true;
AxisMode axisMode = AXIS_XY;
bool gravityEnabled = true;
float gravityStrength = 9.8f;

int zCounter = 1;

// Console
std::vector<std::string> consoleLog;
std::vector<std::string> consoleHistory;
int historyIndex = -1;
std::string consoleInput;
bool consoleActive = false;

//--------------------------------------------------
// Windows
//--------------------------------------------------
Window optionsWin{ {20,40},{260,220},"Options" };
Window inspectWin{ {300,40},{260,220},"Inspector" };
Window sceneWin{ {20,280},{760,400},"Scene" };
Window filesWin{ {800,280},{240,400},"FileExplorer" };
Window consoleWin{ {1050,280},{210,400},"Console" };

//--------------------------------------------------
// Helpers
//--------------------------------------------------
bool MouseIn(Rectangle r) {
    return CheckCollisionPointRec(GetMousePosition(), r);
}

void BringToFront(Window& w) {
    w.zOrder = zCounter++;
}

float Snap(float v) {
    return std::roundf(v / UI_SNAP) * UI_SNAP;
}

//--------------------------------------------------
// Layout Persistence
//--------------------------------------------------
void SaveLayout(const std::vector<Window*>& windows) {
    std::ofstream f("layout.txt", std::ios::trunc);
    for (auto* w : windows)
        f << w->title << " "
        << (int)w->pos.x << " " << (int)w->pos.y << " "
        << (int)w->size.x << " " << (int)w->size.y << " "
        << (w->open ? 1 : 0) << "\n";
}

void LoadLayout(const std::vector<Window*>& windows) {
    std::ifstream f("layout.txt");
    std::string name;
    int x, y, w, h, o;
    while (f >> name >> x >> y >> w >> h >> o)
        for (auto* win : windows)
            if (win->title == name) {
                win->pos = { (float)x,(float)y };
                win->size = { (float)w,(float)h };
                win->open = o == 1;
            }
}

//--------------------------------------------------
// Init
//--------------------------------------------------
void InitGrid() {
    for (int y = 0; y < GRID_SIZE; y++)
        for (int x = 0; x < GRID_SIZE; x++) {
            auto& t = grid[y * GRID_SIZE + x];
            t.pos = { (float)x,(float)y };
            t.type = EMPTY;
        }

    consoleLog.push_back("TechTitan Editor Console");
    consoleLog.push_back("Type 'help' for commands.");
}

//--------------------------------------------------
// Window Handling
//--------------------------------------------------
void HandleWindow(Window& w) {
    if (!w.open) return;

    Vector2 md = GetMouseDelta();

    Rectangle header{ w.pos.x,w.pos.y,w.size.x,TITLE_BAR_HEIGHT };
    Rectangle handle{
        w.pos.x + (w.size.x - DRAG_HANDLE_WIDTH) * 0.5f,
        w.pos.y + 4,DRAG_HANDLE_WIDTH,TITLE_BAR_HEIGHT - 8
    };
    Rectangle resize{ w.pos.x + w.size.x - 12,w.pos.y + w.size.y - 12,12,12 };
    Rectangle close{ w.pos.x + w.size.x - 20,w.pos.y + 5,14,14 };

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && MouseIn(header))
        BringToFront(w);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && MouseIn(close))
        w.open = false;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && MouseIn(handle))
        w.dragging = true;

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && MouseIn(resize))
        w.resizing = true;

    if (w.dragging) {
        w.pos.x += md.x;
        w.pos.y += md.y;
    }

    if (w.resizing) {
        w.size.x = std::max(MIN_WINDOW_W, w.size.x + md.x);
        w.size.y = std::max(MIN_WINDOW_H, w.size.y + md.y);
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        if (w.dragging) {
            w.pos.x = Snap(w.pos.x);
            w.pos.y = Snap(w.pos.y);
        }
        w.dragging = w.resizing = false;
    }
}

//--------------------------------------------------
// Window Drawing
//--------------------------------------------------
void DrawWindow(Window& w) {
    DrawRectangleV(w.pos, w.size, LIGHTGRAY);
    DrawRectangleLines((int)w.pos.x, (int)w.pos.y, (int)w.size.x, (int)w.size.y, BLACK);
    DrawRectangle((int)w.pos.x, (int)w.pos.y, (int)w.size.x, (int)TITLE_BAR_HEIGHT, DARKGRAY);

    Rectangle handle{
        w.pos.x + (w.size.x - DRAG_HANDLE_WIDTH) * 0.5f,
        w.pos.y + 4,DRAG_HANDLE_WIDTH,TITLE_BAR_HEIGHT - 8
    };
    DrawRectangleRec(handle, Fade(BLACK, 0.6f));

    DrawText(w.title.c_str(), (int)w.pos.x + 6, (int)w.pos.y + 6, 14, WHITE);
    DrawRectangle((int)(w.pos.x + w.size.x - 20), (int)(w.pos.y + 5), 14, 14, RED);
}

//--------------------------------------------------
// Scene Toolbar
//--------------------------------------------------
void DrawSceneToolbar(Window& w) {
    const char* labels[] = { "Wall","Player","Exit","Erase" };
    TileType tools[] = { WALL,PLAYER,EXIT,ERASER };

    float x = w.pos.x + 6;
    float y = w.pos.y + TITLE_BAR_HEIGHT + 4;

    for (int i = 0; i < 4; i++) {
        Rectangle r{ x + i * 74.0f,y,70,22 };
        DrawRectangleRec(r, currentTool == tools[i] ? DARKBLUE : GRAY);
        DrawRectangleLines((int)r.x, (int)r.y, (int)r.width, (int)r.height, BLACK);
        DrawText(labels[i], (int)r.x + 6, (int)r.y + 4, 14, WHITE);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && MouseIn(r))
            currentTool = tools[i];
    }
}

//--------------------------------------------------
// Scene
//--------------------------------------------------
void DrawScene(Window& w) {
    DrawSceneToolbar(w);

    Rectangle area{
        w.pos.x + 6,
        w.pos.y + TITLE_BAR_HEIGHT + TOOLBAR_HEIGHT + 6,
        w.size.x - 12,
        w.size.y - TITLE_BAR_HEIGHT - TOOLBAR_HEIGHT - 12
    };

    DrawRectangleRec(area, RAYWHITE);

    if (!showGrid) return;

    float tile = fminf(area.width, area.height) / GRID_SIZE;
    float ox = area.x + (area.width - tile * GRID_SIZE) * 0.5f;
    float oy = area.y + (area.height - tile * GRID_SIZE) * 0.5f;

    for (auto& t : grid) {
        Color c = WHITE;
        if (t.type == WALL) c = RED;
        if (t.type == PLAYER) c = BLUE;
        if (t.type == EXIT) c = GREEN;

        DrawRectangle((int)(ox + t.pos.x * tile), (int)(oy + t.pos.y * tile),
            (int)(tile - 1), (int)(tile - 1), c);
    }

    Vector2 m = GetMousePosition();
    int gx = (int)((m.x - ox) / tile);
    int gy = (int)((m.y - oy) / tile);
    if (gx >= 0 && gy >= 0 && gx < GRID_SIZE && gy < GRID_SIZE)
        DrawRectangle((int)(ox + gx * tile), (int)(oy + gy * tile),
            (int)(tile - 1), (int)(tile - 1), Fade(YELLOW, 0.5f));
}

//--------------------------------------------------
// Scene Input
//--------------------------------------------------
void HandleSceneInput(Window& w) {
    if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON))return;

    Rectangle area{
        w.pos.x + 6,
        w.pos.y + TITLE_BAR_HEIGHT + TOOLBAR_HEIGHT + 6,
        w.size.x - 12,
        w.size.y - TITLE_BAR_HEIGHT - TOOLBAR_HEIGHT - 12
    };

    if (!CheckCollisionPointRec(GetMousePosition(), area))return;

    float tile = fminf(area.width, area.height) / GRID_SIZE;
    float ox = area.x + (area.width - tile * GRID_SIZE) * 0.5f;
    float oy = area.y + (area.height - tile * GRID_SIZE) * 0.5f;

    Vector2 m = GetMousePosition();
    int gx = (int)((m.x - ox) / tile);
    int gy = (int)((m.y - oy) / tile);
    if (gx < 0 || gy < 0 || gx >= GRID_SIZE || gy >= GRID_SIZE)return;

    int idx = gy * GRID_SIZE + gx;
    grid[idx].type = (currentTool == ERASER) ? EMPTY : currentTool;
}

//--------------------------------------------------
// Console Commands
//--------------------------------------------------
void ExecuteConsoleCommand(const std::string& cmd) {
    if (cmd.empty())return;
    consoleLog.push_back("> " + cmd);
    consoleHistory.push_back(cmd);
    historyIndex = (int)consoleHistory.size();

    if (cmd == "help") {
        consoleLog.push_back("help, clear, run_engine");
    }
    else if (cmd == "clear") {
        consoleLog.clear();
    }
    else if (cmd == "run_engine") {
        consoleLog.push_back("[engine] launching...");
        consoleLog.push_back("[engine] (stdout redirection stub)");
    }
    else {
        consoleLog.push_back("Unknown command.");
    }
}

//--------------------------------------------------
// Console UI
//--------------------------------------------------
void DrawConsole(Window& w) {
    Rectangle log{ w.pos.x + 6,w.pos.y + TITLE_BAR_HEIGHT + 6,
                  w.size.x - 12,w.size.y - TITLE_BAR_HEIGHT - 36 };
    Rectangle input{ w.pos.x + 6,w.pos.y + w.size.y - 26,w.size.x - 12,20 };

    DrawRectangleRec(log, RAYWHITE);
    DrawRectangleLines((int)log.x, (int)log.y, (int)log.width, (int)log.height, BLACK);

    int maxLines = (int)(log.height / 16);
    int start = std::max(0, (int)consoleLog.size() - maxLines);
    float y = log.y + 4;

    for (int i = start; i < (int)consoleLog.size(); i++) {
        DrawText(consoleLog[i].c_str(), (int)log.x + 4, (int)y, 14, BLACK);
        y += 16;
    }

    DrawRectangleRec(input, WHITE);
    DrawRectangleLines((int)input.x, (int)input.y, (int)input.width, (int)input.height, BLACK);
    DrawText(consoleInput.c_str(), (int)input.x + 4, (int)input.y + 3, 14, BLACK);
}

//--------------------------------------------------
// Options (Level Settings)
//--------------------------------------------------
void DrawOptions(Window& w) {
    float x = w.pos.x + 10;
    float y = w.pos.y + TITLE_BAR_HEIGHT + 10;
    float line = 24;

    DrawText("Level Options", (int)x, (int)y, 16, BLACK);
    y += line + 6;

    // Show Grid
    Rectangle gridToggle{ x, y, w.size.x - 20, 20 };
    DrawRectangleRec(gridToggle, showGrid ? DARKGREEN : GRAY);
    DrawText(showGrid ? "Grid: ON" : "Grid: OFF",
        (int)x + 6, (int)y + 4, 14, WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && MouseIn(gridToggle))
        showGrid = !showGrid;

    y += line;

    // Gravity toggle
    Rectangle gravToggle{ x, y, w.size.x - 20, 20 };
    DrawRectangleRec(gravToggle, gravityEnabled ? DARKBLUE : GRAY);
    DrawText(gravityEnabled ? "Gravity: ON" : "Gravity: OFF",
        (int)x + 6, (int)y + 4, 14, WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && MouseIn(gravToggle))
        gravityEnabled = !gravityEnabled;

    y += line;

    // Axis mode
    DrawText("Axis Mode:", (int)x, (int)y, 14, BLACK);
    y += line - 6;

    Rectangle axisBtn{ x, y, w.size.x - 20, 20 };
    DrawRectangleRec(axisBtn, GRAY);
    DrawText(axisMode == AXIS_XY ? "XY (2D)" : "XZ (3D)",
        (int)x + 6, (int)y + 4, 14, WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && MouseIn(axisBtn)) {
        axisMode = (axisMode == AXIS_XY) ? AXIS_XZ : AXIS_XY;
    }
}


//--------------------------------------------------
// Console Input
//--------------------------------------------------
void HandleConsoleInput(Window& w) {
    Rectangle input{ w.pos.x + 6,w.pos.y + w.size.y - 26,w.size.x - 12,20 };

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        consoleActive = MouseIn(input);

    if (!consoleActive)return;

    int c;
    while ((c = GetCharPressed()) > 0) {
        if (c >= 32 && c <= 125) consoleInput.push_back((char)c);
    }

    if (IsKeyPressed(KEY_BACKSPACE) && !consoleInput.empty())
        consoleInput.pop_back();

    if (IsKeyPressed(KEY_UP) && !consoleHistory.empty()) {
        historyIndex = std::max(0, historyIndex - 1);
        consoleInput = consoleHistory[historyIndex];
    }

    if (IsKeyPressed(KEY_DOWN) && !consoleHistory.empty()) {
        historyIndex = std::min((int)consoleHistory.size() - 1, historyIndex + 1);
        consoleInput = consoleHistory[historyIndex];
    }

    if (IsKeyPressed(KEY_TAB)) {
        if (consoleInput == "r") consoleInput = "run_engine";
        if (consoleInput == "h") consoleInput = "help";
    }

    if (IsKeyPressed(KEY_ENTER)) {
        ExecuteConsoleCommand(consoleInput);
        consoleInput.clear();
    }
}

//--------------------------------------------------
// Main
//--------------------------------------------------
int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "TechTitan Editor");
    SetTargetFPS(60);
    InitGrid();

    std::vector<Window*> windows{
        &optionsWin,&inspectWin,&sceneWin,&filesWin,&consoleWin
    };

    LoadLayout(windows);

    while (!WindowShouldClose()) {
        std::sort(windows.begin(), windows.end(),
            [](Window* a, Window* b) {return a->zOrder < b->zOrder; });

        for (auto* w : windows) HandleWindow(*w);
        HandleSceneInput(sceneWin);
        HandleConsoleInput(consoleWin);

        BeginDrawing();
        ClearBackground(DARKGRAY);

        for (auto* w : windows) {
            if (!w->open) continue;
            DrawWindow(*w);

            if (w == &sceneWin)   DrawScene(*w);
            if (w == &optionsWin) DrawOptions(*w);
        }
        EndDrawing();
    }

    SaveLayout(windows);
    CloseWindow();
    return 0;
}
