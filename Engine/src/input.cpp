#include "input.h"

void Input::Update(float deltaTime)
{
    MoveAxis = { 0.0f, 0.0f };

    if (IsKeyDown(KEY_LEFT))
        MoveAxis.x -= 1.0f;
    if (IsKeyDown(KEY_RIGHT))
        MoveAxis.x += 1.0f;

    if (IsKeyDown(KEY_UP))
        MoveAxis.y -= 1.0f;
    if (IsKeyDown(KEY_DOWN))
        MoveAxis.y += 1.0f;

    Attack = IsKeyDown(KEY_LEFT_CONTROL);
    Interact = IsKeyDown(KEY_SPACE);
    Run = IsKeyDown(KEY_LEFT_SHIFT);
    Pause = IsKeyPressed(KEY_ENTER);
} 
