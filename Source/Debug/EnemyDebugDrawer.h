// #pragma once
// #include "../Math.h"
//
// class Renderer;
// class Enemy;
// class Game;
//
// class EnemyDebugDrawer
// {
// public:
//     // Main entry point for drawing all enemy debug visualization
//     static void Draw(Renderer* renderer, const Enemy* enemy, Game* game);
//
// private:
//     // State-specific visualization methods
//     static void DrawPatrolState(Renderer* renderer, const Enemy* enemy, Game* game, bool playerDetected);
//     static void DrawChaseState(Renderer* renderer, const Enemy* enemy, Game* game, bool playerDetected);
//     static void DrawSearchState(Renderer* renderer, const Enemy* enemy, Game* game);
//     static void DrawAttackState(Renderer* renderer, const Enemy* enemy, Game* game, bool playerDetected);
//     static void DrawReturningToPatrolState(Renderer* renderer, const Enemy* enemy, Game* game, bool playerDetected);
//
//     // Shared drawing primitives
//     static void DrawDetectionCone(Renderer* renderer, const Vector2& position, const Vector2& forward,
//                                    float radius, float angle, const Vector3& color, const Vector2& cameraPos);
//     static void DrawCircle(Renderer* renderer, const Vector2& position, float radius,
//                            const Vector3& color, const Vector2& cameraPos, int segments = 32);
//     static void DrawProximityCircle(Renderer* renderer, const Vector2& position, float radius,
//                                     const Vector2& cameraPos);
//     static void DrawAttackCircle(Renderer* renderer, const Vector2& position, float radius,
//                                  const Vector2& cameraPos);
//     static void DrawPatrolWaypoints(Renderer* renderer, const Vector2 waypoints[2],
//                                     int currentWaypoint, bool isPatrolling, const Vector2& cameraPos);
//     static void DrawLastKnownPosition(Renderer* renderer, const Vector2& position,
//                                       const Vector2& enemyPos, const Vector2& cameraPos);
// };
