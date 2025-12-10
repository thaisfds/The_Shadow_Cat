// #pragma once
//
// #include "Actor.h"
// #include "Characters/Enemy.h"
// #include "../Math.h"
//
// /**
//  * Spawner - Delayed enemy spawn trigger
//  *
//  * Spawners are invisible actors placed in levels that spawn enemies when the player approaches.
//  * They trigger when the player's camera viewport comes within mSpawnDistance of the spawner position.
//  *
//  * Level CSV Tile IDs:
//  *   14 = Spawner with small patrol (100px left/right from spawn point)
//  *   15 = Spawner with large patrol (200px left/right from spawn point)
//  *
//  * Each spawner only spawns once per level load. Use spawners for:
//  *   - Ambushes in distant areas
//  *   - Performance optimization (don't load all enemies at start)
//  *   - Dynamic difficulty (enemies appear as player explores)
//  *
//  * Compare with immediate enemy spawns (tile IDs 12-13) which spawn when level loads.
//  */
// class Spawner : public Actor
// {
// public:
//     /**
//      * Constructor
//      * @param game Game instance
//      * @param patrolPointA First patrol waypoint for spawned enemy
//      * @param patrolPointB Second patrol waypoint for spawned enemy
//      * @param enemyType Type of enemy to spawn
//      * @param spawnDistance Distance from camera viewport to trigger spawn (default: 700px)
//      */
//     Spawner(Game *game, Vector2 patrolPointA, Vector2 patrolPointB, Enemy::EnemyType enemyType = Enemy::EnemyType::WhiteCat, float spawnDistance = 700.0f);
//
//     void OnUpdate(float deltaTime) override;
//
//     /**
//      * Check if this spawner has already spawned its enemy
//      * @return true if enemy has been spawned, false otherwise
//      */
//     bool HasSpawned() const { return mHasSpawned; }
//
// private:
//     /**
//      * Check if player camera is near enough to trigger spawn
//      * @return true if player is within spawn distance of this spawner
//      */
//     bool IsPlayerNearby() const;
//
//     /**
//      * Spawn the enemy at this spawner's position
//      */
//     void SpawnEnemy();
//
//     float mSpawnDistance;
//     Vector2 mPatrolWaypoints[2];
//     bool mHasSpawned;
//     Enemy::EnemyType mEnemyType;
// };
