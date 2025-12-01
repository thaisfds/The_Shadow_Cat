#pragma once

#include "Actor.h"
#include "../Math.h"

class Spawner : public Actor
{
public:
    Spawner(Game *game, Vector2 patrolPointA, Vector2 patrolPointB, float spawnDistance = 700.0f);

    void OnUpdate(float deltaTime) override;
    
    bool HasSpawned() const { return mHasSpawned; }

private:
    bool IsPlayerNearby() const;
    void SpawnEnemy();
    
    float mSpawnDistance;
    Vector2 mPatrolWaypoints[2];
    bool mHasSpawned;
};
