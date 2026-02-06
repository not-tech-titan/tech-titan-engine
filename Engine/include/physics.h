#pragma once
#include "entity.h"
#include "raylib.h"

namespace Physics
{
    // Check collision between two entities
    bool CheckCollision(const Entity& a, const Entity& b);
    // helper for raw rectangles
    bool CheckCollision(const Rectangle& rec1, const Rectangle& rec2);
    //helper for entity and rectangle
    bool CheckCollision(const Entity& entity, const Rectangle& rect);

    // Resolve collision between two entities
    void ResolveCollision(Entity& a, Entity& b);
}
