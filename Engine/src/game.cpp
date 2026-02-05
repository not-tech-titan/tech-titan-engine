#include "game.h"
#include "entity.h"
#include "settings.h"

Game::Game(Settings& settings) : settings(&settings) {}  // store pointer to settings

Game::~Game() {
    for (Entity* entity : entities) {
        delete entity;
    }
    entities.clear();
}

void Game::Update(float dt) {
    for (Entity* entity : entities) {
        entity->Update(dt);
    }
}

void Game::Draw() {
    for (Entity* entity : entities) {
        entity->Draw();
    }
}

void Game::SpawnEntity(Entity* entity) {
    entities.push_back(entity);
}

void Game::RemoveEntity(Entity* entity) {
    for (size_t i = 0; i < entities.size(); ++i) {
        if (entities[i] == entity) {
            entities.erase(entities.begin() + i);
            return;
        }
    }
}

std::vector<Entity*> Game::GetEntities() const {
    return entities;
}
