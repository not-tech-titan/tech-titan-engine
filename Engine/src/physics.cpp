#include "physics.h"
#include "raymath.h"

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

    // helper for entity and rectangle
    bool CheckCollision(const Entity& entity, const Rectangle& rect)
    {
        Rectangle entityRect = { entity.position.x, entity.position.y, entity.size.x, entity.size.y };
        return CheckCollisionRecs(entityRect, rect);
    }

    void ResolveCollision(Entity& a, Entity& b)
    {
        // Simple elastic collision resolution. Temporary as fuck
        Vector3 normal = { b.position.x - a.position.x, b.position.y - a.position.y, 0.0f };
        float length = sqrt(normal.x * normal.x + normal.y * normal.y);
        if (length == 0) return; // Prevent division by zero
        normal.x /= length;
        normal.y /= length;

        float relativeVelocityX = b.velocity.x - a.velocity.x;
        float relativeVelocityY = b.velocity.y - a.velocity.y;
        float velocityAlongNormal = relativeVelocityX * normal.x + relativeVelocityY * normal.y;

        if (velocityAlongNormal > 0) return; // They are moving apart

        float restitution = 0.5f; // Coefficient of restitution (elasticity)
        float impulseScalar = -(1 + restitution) * velocityAlongNormal;
        impulseScalar /= (1 / a.friction) + (1 / b.friction);

        Vector3 impulse = { impulseScalar * normal.x, impulseScalar * normal.y, 0.0f };

        a.velocity.x -= impulse.x / a.friction;
        a.velocity.y -= impulse.y / a.friction;
        b.velocity.x += impulse.x / b.friction;
        b.velocity.y += impulse.y / b.friction;
    }
}
