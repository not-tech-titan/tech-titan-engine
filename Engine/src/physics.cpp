#include "physics.h"

namespace Physics
{
    // Collision between two entities
    bool CheckCollision(const Entity& a, const Entity& b)
    {
        Rectangle recA = { a.position.x, a.position.y, a.size.x, a.size.y };
        Rectangle recB = { b.position.x, b.position.y, b.size.x, b.size.y };

        return CheckCollisionRecs(recA, recB);
    }

    // helper for raw rectangles
    bool CheckCollision(const Rectangle& rec1, const Rectangle& rec2)
    {
        return ::CheckCollisionRecs(rec1, rec2);
    }
}
