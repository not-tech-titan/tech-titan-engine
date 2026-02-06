#define _CRT_SECURE_NO_WARNINGS
#include "raylib.h"
#include "game.h"
#include "settings.h"
#include "input.h"
#include "entity.h"
#include "physics.h"
#include "console.h"

int main() 
{
    Console::PrintLine("TechTitan Engine - Space Storm Demo");
    // Load & Apply Settings
    Settings settings;
    settings.Load();

    // Initialize Window
    InitWindow(settings.video.windowWidth, settings.video.windowHeight, "Space Storm");
    SetTargetFPS(settings.video.targetFPS);

    // Apply video and audio settings AFTER window/audio initialization
    settings.ApplyVideo();
    settings.ApplyAudio();

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

    bool gameStarted = false;
    bool isPaused = false;
    Console::PrintLine("Game Started!");

    while (!WindowShouldClose()) 
    {
        // Pausing
        float dt = GetFrameTime();
        if (Input::GetButtonPressed("Pause")) 
        {
            isPaused = !isPaused;
            if(isPaused)
                Console::PrintLine("Game Paused.");
            else
                Console::PrintLine("Game Resumed.");
        }
        if (!isPaused) 
        {
            game.Update(dt);

            // Temporary game logic for testing

            //stars effect (experimental, will be replaced with particle system eventually)
            //Initial stars
            if (!gameStarted)
            {
                for (int i = 0; i < 50; ++i) 
                {
                    Entity* star = new Entity({(float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight()), 0}, {2, 2, 1}, GRAY);
                    game.SpawnEntity(star);
                    star->AddForce({0, (float)GetRandomValue(150, 300), 0});
                }
                gameStarted = true;
            }
            
            // Spawn new stars at the top randomly
            if (GetRandomValue(0, 100) < 25) 
            {
                Entity* star = new Entity({(float)GetRandomValue(0, GetScreenWidth()), -10, 0}, {2, 2, 1}, GRAY);
                game.SpawnEntity(star);
                star->AddForce({0, (float)GetRandomValue(150, 300), 0});
            }

            // Move the player based on input
            player->AddForce({Input::GetVector2("Move").x * 50, Input::GetVector2("Move").y * 50, 0});
            // If player pressed fire, shoot
            shootTimer += dt;
            if (Input::GetButton("Fire") && shootTimer >= 0.35f) 
            {
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
            Rectangle enemyView = {enemy->position.x, enemy->position.y + enemy->size.y, enemy->size.x, enemy->size.y * 32};
            if (Physics::CheckCollision(*player, enemyView) && AIShootTimer >= 0.35f) 
            {
                Entity* enemy_projectile = new Entity({enemy->position.x + 10, enemy->position.y + 30, 0}, {5, 10, 1}, YELLOW);
                game.SpawnEntity(enemy_projectile);
                enemy_projectile->AddForce({0, 750, 0});
                AIShootTimer = 0.0f;
            }

            // Testing collision resolution
            if( Physics::CheckCollision(*player, *enemy) ) 
            {
                Physics::ResolveCollision(*player, *enemy);
            }

            // Keep player and enemy on screen so they dony despawn (super mega temporary)
            // player left
            if (player->position.x < 0) {player->position.x = 0; player->velocity.x *= -0.5f;}
            // player right
            if (player->position.x > GetScreenWidth() - player->size.x) {player->position.x = GetScreenWidth() - player->size.x; player->velocity.x *= -0.5f;}
            // player top
            if (player->position.y < 0) {player->position.y = 0; player->velocity.y *= -0.5f;}
            // player bottom
            if (player->position.y > GetScreenHeight() - player->size.y) {player->position.y = GetScreenHeight() - player->size.y; player->velocity.y *= -0.5f;}
            // enemy left
            if (enemy->position.x < 0) {enemy->position.x = 0; enemy->velocity.x *= -0.5f;}
            // enemy right
            if (enemy->position.x > GetScreenWidth() - enemy->size.x) {enemy->position.x = GetScreenWidth() - enemy->size.x; enemy->velocity.x *= -0.5f;}
            // enemy top
            if (enemy->position.y < 0) {enemy->position.y = 0; enemy->velocity.y *= -0.5f;}
            // enemy bottom
            if (enemy->position.y > GetScreenHeight() - enemy->size.y) {enemy->position.y = GetScreenHeight() - enemy->size.y; enemy->velocity.y *= -0.5f;}
        }

        Input::Update(); // update all actions

        BeginDrawing();
        ClearBackground(BLACK);

        game.Draw();
        // Draw pause menu
        if (isPaused) 
        {
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