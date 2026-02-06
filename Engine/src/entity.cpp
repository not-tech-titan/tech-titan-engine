#include "entity.h"

Entity::Entity(Vector3 startPos, Vector3 startSize, Color startColor)
{
    position = startPos;
    size = startSize;
    color = startColor; 
    velocity = {0,0,0};
}

void Entity::Update(float deltaTime)
{
    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;
    position.z += velocity.z * deltaTime;

    // friction
    velocity.x *= friction;
    velocity.y *= friction;
    velocity.z *= friction;
}

void Entity::AddForce(Vector3 force)
{
    velocity.x += force.x;
    velocity.y += force.y;
    velocity.z += force.z;
}

void Entity::Draw() const
{
    DrawRectangle(position.x, position.y, size.x, size.y, color);
}
