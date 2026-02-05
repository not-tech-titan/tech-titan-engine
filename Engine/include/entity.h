#pragma once
#include "raylib.h"

class Entity
{
public:
    Vector3 position;
    Vector3 velocity;
    Vector3 size;
    float friction = 1;
    Color color;

    Entity(Vector3 startPos, Vector3 startSize, Color startColor);

    void Update(float deltaTime);
    void Draw() const;
    void AddForce(Vector3 force);
};
