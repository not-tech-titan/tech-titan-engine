#define _CRT_SECURE_NO_WARNINGS
#include "raylib.h"
#include "raymath.h"
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
    int playerScore = 0;
    int enemyScore = 0;
    float AIInputTimer = 0.0f;
    float AIShootTimer = 0.0f;
    InitWindow(screenWidth, screenHeight, "Space Storm");

    SetTargetFPS(60);

    // Manually creating entities for now
    Entity player({ float(GetScreenWidth()) / 2, float(GetScreenHeight() * 0.75f), 0 }, { 25, 25, 1 }, BLUE);
    Entity enemy({ float(GetScreenWidth()) / 2, float(GetScreenHeight() / 4) - 100, 0 }, { 25, 25, 1 }, RED);
    Entity bullet({ 0,0,0 }, { 5, 10, 1 }, YELLOW);
    Entity bullet2({ 0,0,0 }, { 5, 10, 1 }, YELLOW);
    Entity enemyBullet({ 0,float(GetScreenHeight()),0 }, { 5, 10, 1 }, YELLOW);
    Entity enemyBullet2({ 0,float(GetScreenHeight()),0 }, { 5, 10, 1 }, YELLOW);

    // Initializing input
    Input input;

    float shootTimer = 0;
    //game loop
    while (!WindowShouldClose())
    {
        // delta time
        float dt = GetFrameTime();

        shootTimer += dt;
        AIInputTimer += dt;
        AIShootTimer += dt;
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

        // Simple AI for enemy movement (move in a circle)
        enemy.position.x += sin(AIInputTimer) * 2;
        enemy.position.y += cos(AIInputTimer);
        bool AIShoot = Physics::CheckCollision({enemy.position.x - enemy.size.x,enemy.position.y,enemy.size.x * 4,enemy.size.y * 50},{player.position.x,player.position.y,player.size.x,player.size.y});
        if (AIShoot && AIShootTimer >= 0.35f )
        {
            // Shoot if we can "see the player"
            if (enemyBullet.position.y >= GetScreenHeight() + 64)
            {
                enemyBullet.position.x = enemy.position.x + (enemy.size.x / 2);
                enemyBullet.position.y = enemy.position.y + (enemy.size.y / 2);
            }
            else if(enemyBullet2.position.y >= GetScreenHeight() + 64)
            {
                enemyBullet2.position.x = enemy.position.x + (enemy.size.x / 2);
                enemyBullet2.position.y = enemy.position.y + (enemy.size.y / 2);
            }
            AIShootTimer = 0;
        }

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
        // Move bullets
        bullet.position.y -= 1000 * dt;
        bullet2.position.y -= 1000 * dt;
        enemyBullet.position.y += 1000 * dt;
        enemyBullet2.position.y += 1000 * dt;
        // If bullets and enemy are colliding, move the bullet above the screen and move the enemy somewhere else
        // bullet 1
        if (Physics::CheckCollision(bullet,enemy))
        {
            enemy.position.x = GetRandomValue(64, GetScreenWidth() - 64);
            enemy.position.y = GetRandomValue(100,300);
            bullet.position.y = -64;
            playerScore++;
        }
        // bullet 2
        if (Physics::CheckCollision(bullet2, enemy))
        {
            enemy.position.x = GetRandomValue(64, GetScreenWidth() - 64);
            enemy.position.y = GetRandomValue(100, 300);
            bullet2.position.y = -64;
            playerScore++;
        }
        // enemy bullet 1
        if (Physics::CheckCollision(enemyBullet, player))
        {
            player.position.x = GetRandomValue(64, GetScreenWidth() - 64);
            player.position.y = GetRandomValue(450,700);
            enemyBullet.position.y = GetScreenHeight() + 64;
            enemyScore++;
        }
        // enemy bullet 2
        if (Physics::CheckCollision(enemyBullet2, player))
        {
            player.position.x = GetRandomValue(64, GetScreenWidth() - 64);
            player.position.y = GetRandomValue(450, 700);
            enemyBullet2.position.y = GetScreenHeight() + 64;
            enemyScore++;
        }
        //Update our entites (gamemanager will do this later)
        player.Update(dt);
        enemy.Update(dt);
        bullet.Update(dt);
        bullet2.Update(dt);
        enemyBullet.Update(dt);
        enemyBullet2.Update(dt);

        //Initialize drawing
        BeginDrawing();
        ClearBackground(BLACK);

        //Draw our entities (gamemanager will do this later)
        bullet.Draw();
        bullet2.Draw();
        enemyBullet.Draw();
        enemyBullet2.Draw();
        player.Draw();
        enemy.Draw();

        char scoreStr[32]; // make sure it's big enough for text + number
        sprintf(scoreStr, "Player Score: %d", playerScore);
        DrawText(scoreStr, 10, 10, 32, WHITE);

        char enemyScoreStr[32]; // make sure it's big enough for text + number
        sprintf(enemyScoreStr, "Enemy Score: %d", enemyScore);
        DrawText(enemyScoreStr, 10, 42, 32, WHITE);

        //self explanatory
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
