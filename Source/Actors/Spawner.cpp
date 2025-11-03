#include "Spawner.h"
#include "../Game.h"
#include "Mario.h"
#include "Goomba.h"

Spawner::Spawner(Game *game, float spawnDistance)
    : Actor(game), mSpawnDistance(spawnDistance)
{
}

void Spawner::OnUpdate(float deltaTime)
{
}