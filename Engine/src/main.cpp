#define _CRT_SECURE_NO_WARNINGS
#include "raylib.h"
#include "entity.h"
#include "input.h"
#include "physics.h"
#include <string>
#include <stdio.h>


int main()
{
    // will be handles by settings later
    const int screenWidth = 1024;
    const int screenHeight = 720;
    int score = 0;
    InitWindow(screenWidth, screenHeight, "Space Storm");

    SetTargetFPS(60);

    // Manually creating entities for now
    Entity player({ 100, 500, 0 }, { 25, 25, 1 }, BLUE);
    Entity enemy({ 100, 100, 0 }, { 25, 25, 1 }, RED);
    Entity bullet({ 0,0,0 }, { 5, 10, 1 }, YELLOW);
    Entity bullet2({ 0,0,0 }, { 5, 10, 1 }, YELLOW);

    // Initializing input
    Input input;

    float shootTimer = 0;
    //game loop
    while (!WindowShouldClose())
    {
        // delta time
        float dt = GetFrameTime();
        shootTimer += dt;
        // Update Input
        input.Update(dt);

        //temporarily apply input to player
        if (input.Run)
        {
            player.position.x += input.MoveAxis.x * 5;
            player.position.y += input.MoveAxis.y * 5;
        }
        player.position.x += input.MoveAxis.x * 5;
        player.position.y += input.MoveAxis.y * 5;

        //Bullet input and update (testing)
        if (input.Attack && shootTimer >= 0.35f)
        {
            if (bullet.position.y <= -64)
            {
                bullet.position.x = player.position.x + (player.size.x / 2);
                bullet.position.y = player.position.y + (player.size.y / 2);
            }
            else if(bullet2.position.y <= -64)
            {
                bullet2.position.x = player.position.x + (player.size.x / 2);
                bullet2.position.y = player.position.y + (player.size.y / 2);
            }
            shootTimer = 0;
        }
        bullet.position.y -= 1000 * dt;
        bullet2.position.y -= 1000 * dt;
        // If bullets and enemy are colliding, move the bullet above the screen and move the enemy somewhere else
        // bullet 1
        if (Physics::CheckCollision(bullet,enemy))
        {
            enemy.position.x = GetRandomValue(64, GetScreenWidth() - 64);
            enemy.position.y = GetRandomValue(100,300);
            bullet.position.y = -64;
            score += 100;
        }
        // bullet 2
        if (Physics::CheckCollision(bullet2, enemy))
        {
            enemy.position.x = GetRandomValue(64, GetScreenWidth() - 64);
            enemy.position.y = GetRandomValue(100, 300);
            bullet2.position.y = -64;
            score += 100;
        }
        //Update our entites (gamemanager will do this later)
        player.Update(dt);
        enemy.Update(dt);
        bullet.Update(dt);
        bullet2.Update(dt);

        //Initialize drawing
        BeginDrawing();
        ClearBackground(BLACK);

        //Draw our entities (gamemanager will do this later)
        bullet.Draw();
        bullet2.Draw();
        player.Draw();
        enemy.Draw();

        char scoreStr[32]; // make sure it's big enough for text + number
        sprintf(scoreStr, "Score: %d", score);
        DrawText(scoreStr, 10, 10, 32, WHITE);

        //self explanatory
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
