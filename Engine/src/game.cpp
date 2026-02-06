#include "game.h"
#include "entity.h"
#include "settings.h"
#include "console.h"

Game::Game(Settings& settings) : settings(&settings) {}  // store pointer to settings

Game::~Game() 
{
    for (Entity* entity : entities) 
    {
        delete entity;
    }
    entities.clear();
}

void Game::Update(float dt) 
{
    for (Entity* entity : entities) 
    {
        entity->Update(dt);
        // delete if off-screen drastically (temporary)
        if (entity->position.x < -GetScreenWidth() || entity->position.x > GetScreenWidth() * 2 ||
            entity->position.y < -GetScreenHeight() || entity->position.y > GetScreenHeight() * 2) 
        {
            RemoveEntity(entity);
            delete entity;
        }
    }
}

void Game::Draw() 
{
    for (Entity* entity : entities) 
    {
        entity->Draw();
    }
}

void Game::SpawnEntity(Entity* entity) 
{
    entities.push_back(entity);
}

void Game::RemoveEntity(Entity* entity) 
{
    for (size_t i = 0; i < entities.size(); ++i) 
    {
        if (entities[i] == entity) {
            entities.erase(entities.begin() + i);
            return;
        }
    }
}

std::vector<Entity*> Game::GetEntities() const 
{
    return entities;
}
