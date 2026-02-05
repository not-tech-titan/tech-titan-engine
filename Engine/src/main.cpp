#define _CRT_SECURE_NO_WARNINGS
#include "raylib.h"
#include "game.h"
#include "settings.h"
#include "input.h"
#include "entity.h"
#include "physics.h"

int main() 
{
    // Load & Apply Settings
    Settings settings;
    settings.Load();
    settings.ApplyVideo();
    settings.ApplyAudio();

    InitWindow(settings.video.windowWidth, settings.video.windowHeight, "Space Storm");
    SetTargetFPS(settings.video.targetFPS);

    // Initialize Input
    Input::Init();

    // Register actions (these will eventually come from editor-defined input)
    Input::RegisterVector2("Move");
    Input::RegisterButton("Fire");
    Input::RegisterButton("Pause");

    // Bind keys (can be loaded from settings.controls later)
    Input::BindKey("Fire", KEY_SPACE);
    Input::BindKey("Pause", KEY_ENTER);
    Input::BindVector2("Move", KEY_A, KEY_D, KEY_W, KEY_S);

    // Create Game instance
    Game game(settings);

    // Spawn initial entities. for testing
    Entity* player = new Entity({400, 500, 0}, {25,25,1}, BLUE);
    game.SpawnEntity(player);
    player->friction = 0.9f;

    float shootTimer = 0.0f;

    Entity* enemy = new Entity({200, 100, 0}, {25,25,1}, RED);
    game.SpawnEntity(enemy);
    enemy->friction = 0.95f;

    float AITimer = 0.0f;
    float AIShootTimer = 0.0f;

    // Main loop
    bool isPaused = false;
    while (!WindowShouldClose()) 
    {
        // Pausing
        float dt = GetFrameTime();
        if (Input::GetButtonPressed("Pause")) {
            isPaused = !isPaused;
        }
        if (!isPaused) {
            game.Update(dt);

            // Temporary game logic for testing

            // Move the player based on input
            player->AddForce({Input::GetVector2("Move").x * 50, Input::GetVector2("Move").y * 50, 0});\
            // If player pressed fire, shoot
            shootTimer += dt;
            if (Input::GetButton("Fire") && shootTimer >= 0.35f) {
                Entity* projectile = new Entity({player->position.x + 10, player->position.y - 13, 0}, {5, 10, 1}, YELLOW);
                game.SpawnEntity(projectile);
                projectile->AddForce({0, -750, 0});
                shootTimer = 0.0f;
            }

            // Move the enemy left and right
            AITimer += dt;
            enemy->AddForce({sin(AITimer) * 10, 0, 0});
            // if enemy can see player, shoot
            AIShootTimer += dt;
            if (Physics::CheckCollision({enemy->position.x, enemy->position.y + enemy->size.y, enemy->size.x, enemy->size.y * 32}, {player->position.x, player->position.y, player->size.x, player->size.y}) && AIShootTimer >= 0.35f) {
                Entity* enemy_projectile = new Entity({enemy->position.x + 10, enemy->position.y + 30, 0}, {5, 10, 1}, YELLOW);
                game.SpawnEntity(enemy_projectile);
                enemy_projectile->AddForce({0, 750, 0});
                AIShootTimer = 0.0f;
            }

            // Testing collision resolution
            if( Physics::CheckCollision(*player, *enemy) ) {
                Physics::ResolveCollision(*player, *enemy);
            }
        }

        Input::Update(); // update all actions

        BeginDrawing();
        ClearBackground(BLACK);

        game.Draw();
        // Draw pause menu
        if (isPaused) {
            int screenW = GetScreenWidth();
            int screenH = GetScreenHeight();

            // Pause overlay size
            int overlayW = screenW / 4;
            int overlayH = screenH / 4;
            int overlayX = screenW / 2 - overlayW / 2;
            int overlayY = screenH / 2 - overlayH / 2;

            DrawRectangle(overlayX, overlayY, overlayW, overlayH, Fade(WHITE, 0.25f));

            // Text positions
            int titleX = screenW / 2 - MeasureText("PAUSED", 25) / 2;
            int titleY = screenH / 2 - 50;
            int subtitleX = screenW / 2 - MeasureText("Press ENTER to resume", 10) / 2;
            int subtitleY = screenH / 2 + 25;

            DrawText("PAUSED", titleX, titleY, 25, WHITE);
            DrawText("Press ENTER to resume", subtitleX, subtitleY, 10, WHITE);
        }
        EndDrawing();
    }

    // Cleanup
    Input::Shutdown();

    CloseWindow();

    return 0;
}