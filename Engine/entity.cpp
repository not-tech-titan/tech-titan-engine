#include "entity.h"

Entity::Entity(Vector3 startPos, Vector3 startSize, Color startColor)
{
    position = startPos;
    size = startSize;
    color = startColor;
}

void Entity::Update(float deltaTime)
{

}

void Entity::Draw() const
{
    DrawRectangle(position.x, position.y, size.x, size.y, color);
}
