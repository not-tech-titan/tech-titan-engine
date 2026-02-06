#pragma once
#include <vector>
#include "entity.h"
#include "settings.h"

class Game 
{
public:
    // Constructor now takes a reference to Settings
    Game(Settings& settings);  
    ~Game();

    void Update(float dt);
    void Draw();

    void SpawnEntity(Entity* entity);
    void RemoveEntity(Entity* entity);

    std::vector<Entity*> GetEntities() const;

private:
    std::vector<Entity*> entities;
    Settings* settings; // store pointer instead of copy
};
