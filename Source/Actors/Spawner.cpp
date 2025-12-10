// #include "Spawner.h"
// #include "../Game.h"
// #include "../GameConstants.h"
// #include <SDL.h>
//
// Spawner::Spawner(Game *game, Vector2 patrolPointA, Vector2 patrolPointB, Enemy::EnemyType enemyType, float spawnDistance)
//     : Actor(game)
//     , mSpawnDistance(spawnDistance)
//     , mHasSpawned(false)
//     , mEnemyType(enemyType)
// {
//     mPatrolWaypoints[0] = patrolPointA;
//     mPatrolWaypoints[1] = patrolPointB;
//
//     if (mGame->IsDebugging())
//     {
//         SDL_Log("Spawner created at position (%.2f, %.2f) with spawn distance %.2f",
//                 mPosition.x, mPosition.y, mSpawnDistance);
//     }
// }
//
// void Spawner::OnUpdate(float deltaTime)
// {
//     Actor::OnUpdate(deltaTime);
//
//     // Only check if we haven't spawned yet
//     if (!mHasSpawned && IsPlayerNearby())
//     {
//         SpawnEnemy();
//         mHasSpawned = true;
//     }
// }
//
// bool Spawner::IsPlayerNearby() const
// {
//     // Get camera position (top-left corner of viewport)
//     Vector2 cameraPos = mGame->GetCameraPos();
//
//     // Calculate viewport bounds with spawn distance buffer
//     float viewportLeft = cameraPos.x - mSpawnDistance;
//     float viewportRight = cameraPos.x + GameConstants::WINDOW_WIDTH + mSpawnDistance;
//     float viewportTop = cameraPos.y - mSpawnDistance;
//     float viewportBottom = cameraPos.y + GameConstants::WINDOW_HEIGHT + mSpawnDistance;
//
//     // Check if spawner position is within buffered viewport
//     return (mPosition.x >= viewportLeft && mPosition.x <= viewportRight &&
//             mPosition.y >= viewportTop && mPosition.y <= viewportBottom);
// }
//
// void Spawner::SpawnEnemy()
// {
//     if (mGame->IsDebugging())
//     {
//         SDL_Log("Spawner at (%.2f, %.2f) triggering enemy spawn!", mPosition.x, mPosition.y);
//     }
//
//     // Create enemy at spawner position with stored patrol waypoints
//     // Enemy type is determined by the current level in Game.cpp
//     auto enemy = new Enemy(mGame, mPatrolWaypoints[0], mPatrolWaypoints[1], mEnemyType);
//     enemy->SetPosition(mPosition);
//
//     if (mGame->IsDebugging())
//     {
//         SDL_Log("Enemy spawned at (%.2f, %.2f) with waypoints: (%.2f, %.2f) to (%.2f, %.2f)",
//                 mPosition.x, mPosition.y,
//                 mPatrolWaypoints[0].x, mPatrolWaypoints[0].y,
//                 mPatrolWaypoints[1].x, mPatrolWaypoints[1].y);
//     }
// }