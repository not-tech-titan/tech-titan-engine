#pragma once
#include "raylib.h"

class Entity
{
public:
    Vector3 position;
    Vector3 size;
    Color color;

    Entity(Vector3 startPos, Vector3 startSize, Color startColor);

    void Update(float deltaTime);
    void Draw() const;
};
