#pragma once
#include "raylib.h"

class Input
{
public:
    Vector2 MoveAxis{};
    Vector2 AimAxis{};

    bool Attack = false;
    bool Interact = false;
    bool Switch = false;
    bool Run = false;
    bool Jump = false;
    bool Pause = false;

    void Update(float deltaTime);
};
