#include "Spawner.h"
#include "Characters/Enemy.h"
#include "../Game.h"
#include "../GameConstants.h"

Spawner::Spawner(Game *game, Vector2 patrolPointA, Vector2 patrolPointB, float spawnDistance)
    : Actor(game)
    , mSpawnDistance(spawnDistance)
    , mHasSpawned(false)
{
    mPatrolWaypoints[0] = patrolPointA;
    mPatrolWaypoints[1] = patrolPointB;
}

void Spawner::OnUpdate(float deltaTime)
{
    Actor::OnUpdate(deltaTime);
    
    // Only check if we haven't spawned yet
    if (!mHasSpawned && IsPlayerNearby())
    {
        SpawnEnemy();
        mHasSpawned = true;
    }
}

bool Spawner::IsPlayerNearby() const
{
    // Get camera position (top-left corner of viewport)
    Vector2 cameraPos = mGame->GetCameraPos();
    
    // Calculate viewport bounds with spawn distance buffer
    float viewportLeft = cameraPos.x - mSpawnDistance;
    float viewportRight = cameraPos.x + GameConstants::WINDOW_WIDTH + mSpawnDistance;
    float viewportTop = cameraPos.y - mSpawnDistance;
    float viewportBottom = cameraPos.y + GameConstants::WINDOW_HEIGHT + mSpawnDistance;
    
    // Check if spawner position is within buffered viewport
    return (mPosition.x >= viewportLeft && mPosition.x <= viewportRight &&
            mPosition.y >= viewportTop && mPosition.y <= viewportBottom);
}

void Spawner::SpawnEnemy()
{
    // Create enemy at spawner position with stored patrol waypoints
    auto enemy = new Enemy(mGame, mPatrolWaypoints[0], mPatrolWaypoints[1]);
    enemy->SetPosition(mPosition);
}