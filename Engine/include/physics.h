#pragma once
#include "entity.h"
#include "raylib.h"

namespace Physics
{
    // Check collision between two entities
    bool CheckCollision(const Entity& a, const Entity& b);
    // helper for raw rectangles
    bool CheckCollision(const Rectangle& rec1, const Rectangle& rec2);

    void ResolveCollision(Entity& a, Entity& b);
}
