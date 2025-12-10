// #include "EnemyDebugDrawer.h"
// #include "../Renderer/Renderer.h"
// #include "../Game.h"
// #include <SDL.h>
//
// void EnemyDebugDrawer::Draw(Renderer* renderer, const Enemy* enemy, Game* game)
// {
//     if (!game->IsDebugging()) return;
//
//     Vector2 position = enemy->GetPosition();
//     Vector2 cameraPos = game->GetCameraPos();
//     bool playerDetected = enemy->IsPlayerDetected();
//
//     // Draw state-specific visualization
//     switch (enemy->GetCurrentState())
//     {
//         case Enemy::AIState::Patrol:
//             DrawPatrolState(renderer, enemy, game, playerDetected);
//             break;
//
//         case Enemy::AIState::Chase:
//             DrawChaseState(renderer, enemy, game, playerDetected);
//             break;
//
//         case Enemy::AIState::Searching:
//             DrawSearchState(renderer, enemy, game);
//             break;
//
//         case Enemy::AIState::Attack:
//             DrawAttackState(renderer, enemy, game, playerDetected);
//             break;
//
//         case Enemy::AIState::ReturningToPatrol:
//             DrawReturningToPatrolState(renderer, enemy, game, playerDetected);
//             break;
//     }
//
//     // Always draw patrol waypoints for all states
//     DrawPatrolWaypoints(renderer, enemy->GetPatrolWaypoints(),
//                        enemy->GetCurrentWaypoint(),
//                        enemy->GetCurrentState() == Enemy::AIState::Patrol,
//                        cameraPos);
// }
//
// void EnemyDebugDrawer::DrawPatrolState(Renderer* renderer, const Enemy* enemy, Game* game, bool playerDetected)
// {
//     Vector2 position = enemy->GetPosition();
//     Vector2 cameraPos = game->GetCameraPos();
//     Vector3 detectionColor = playerDetected ? Vector3(1.0f, 0.0f, 0.0f) : Vector3(1.0f, 1.0f, 0.0f);
//
//     // Draw detection cone
//     Vector2 forward = enemy->GetForwardDirection();
//     DrawDetectionCone(renderer, position, forward, enemy->GetDetectionRadius(),
//                      enemy->GetDetectionAngle(), detectionColor, cameraPos);
//
//     // Draw proximity circle
//     DrawProximityCircle(renderer, position, enemy->GetProximityRadius(), cameraPos);
// }
//
// void EnemyDebugDrawer::DrawChaseState(Renderer* renderer, const Enemy* enemy, Game* game, bool playerDetected)
// {
//     Vector2 position = enemy->GetPosition();
//     Vector2 cameraPos = game->GetCameraPos();
//     Vector3 detectionColor = playerDetected ? Vector3(1.0f, 0.0f, 0.0f) : Vector3(1.0f, 1.0f, 0.0f);
//
//     // Draw chase circle
//     DrawCircle(renderer, position, enemy->GetChaseDetectionRadius(), detectionColor, cameraPos);
//
//     // Draw proximity circle
//     DrawProximityCircle(renderer, position, enemy->GetProximityRadius(), cameraPos);
// }
//
// void EnemyDebugDrawer::DrawSearchState(Renderer* renderer, const Enemy* enemy, Game* game)
// {
//     Vector2 position = enemy->GetPosition();
//     Vector2 cameraPos = game->GetCameraPos();
//     Vector3 searchColor = Vector3(1.0f, 0.6f, 0.0f); // Orange
//
//     // Draw detection cone (uses chase radius for extended range during search)
//     Vector2 forward = enemy->GetForwardDirection();
//     DrawDetectionCone(renderer, position, forward, enemy->GetChaseDetectionRadius(),
//                      enemy->GetDetectionAngle(), searchColor, cameraPos);
//
//     // Draw proximity circle
//     DrawProximityCircle(renderer, position, enemy->GetProximityRadius(), cameraPos);
//
//     // Draw last known position marker
//     DrawLastKnownPosition(renderer, enemy->GetLastKnownPlayerPos(), position, cameraPos);
// }
//
// void EnemyDebugDrawer::DrawAttackState(Renderer* renderer, const Enemy* enemy, Game* game, bool playerDetected)
// {
//     Vector2 position = enemy->GetPosition();
//     Vector2 cameraPos = game->GetCameraPos();
//     Vector3 attackColor = playerDetected ? Vector3(1.0f, 0.0f, 0.0f) : Vector3(1.0f, 0.0f, 0.5f); // Red or purple
//
//     // Draw attack range circle
//     DrawAttackCircle(renderer, position, enemy->GetAttackRange(), cameraPos);
//
//     // Draw chase circle (enemy will transition to chase if player leaves attack range)
//     DrawCircle(renderer, position, enemy->GetChaseDetectionRadius(), attackColor, cameraPos);
//
//     // Draw proximity circle
//     DrawProximityCircle(renderer, position, enemy->GetProximityRadius(), cameraPos);
// }
//
// void EnemyDebugDrawer::DrawReturningToPatrolState(Renderer* renderer, const Enemy* enemy, Game* game, bool playerDetected)
// {
//     Vector2 position = enemy->GetPosition();
//     Vector2 cameraPos = game->GetCameraPos();
//     Vector3 detectionColor = playerDetected ? Vector3(1.0f, 0.0f, 0.0f) : Vector3(0.5f, 1.0f, 0.5f); // Red or light green
//
//     // Draw chase circle (uses 360° radius detection)
//     DrawCircle(renderer, position, enemy->GetChaseDetectionRadius(), detectionColor, cameraPos);
//
//     // Draw proximity circle (also uses 360° close range detection)
//     DrawProximityCircle(renderer, position, enemy->GetProximityRadius(), cameraPos);
// }
//
// void EnemyDebugDrawer::DrawDetectionCone(Renderer* renderer, const Vector2& position, const Vector2& forward,
//                                          float radius, float angle, const Vector3& color, const Vector2& cameraPos)
// {
//     float baseAngle = Math::Atan2(forward.y, forward.x);
//
//     const int segments = 16;
//     float halfConeAngle = angle / 2.0f;
//     float startAngle = baseAngle - halfConeAngle;
//     float angleStep = angle / segments;
//
//     // Draw the arc
//     for (int i = 0; i < segments; i++)
//     {
//         float angle1 = startAngle + i * angleStep;
//         float angle2 = startAngle + (i + 1) * angleStep;
//
//         Vector2 p1 = position + Vector2(
//             Math::Cos(angle1) * radius,
//             Math::Sin(angle1) * radius
//         );
//
//         Vector2 p2 = position + Vector2(
//             Math::Cos(angle2) * radius,
//             Math::Sin(angle2) * radius
//         );
//
//         renderer->DrawRect(p1, Vector2(4.0f, 4.0f), 0.0f, color, cameraPos, RendererMode::LINES);
//     }
//
//     // Draw cone edges
//     Vector2 leftEdge = position + Vector2(
//         Math::Cos(startAngle) * radius,
//         Math::Sin(startAngle) * radius
//     );
//     Vector2 rightEdge = position + Vector2(
//         Math::Cos(startAngle + angle) * radius,
//         Math::Sin(startAngle + angle) * radius
//     );
//
//     const int edgePoints = 8;
//     for (int i = 0; i <= edgePoints; i++)
//     {
//         float t = i / (float)edgePoints;
//         Vector2 leftPoint = position + (leftEdge - position) * t;
//         Vector2 rightPoint = position + (rightEdge - position) * t;
//
//         renderer->DrawRect(leftPoint, Vector2(4.0f, 4.0f), 0.0f, color, cameraPos, RendererMode::LINES);
//         renderer->DrawRect(rightPoint, Vector2(4.0f, 4.0f), 0.0f, color, cameraPos, RendererMode::LINES);
//     }
// }
//
// void EnemyDebugDrawer::DrawCircle(Renderer* renderer, const Vector2& position, float radius,
//                                   const Vector3& color, const Vector2& cameraPos, int segments)
// {
//     const float angleStep = Math::TwoPi / segments;
//
//     for (int i = 0; i < segments; i++)
//     {
//         float angle = i * angleStep;
//
//         Vector2 p = position + Vector2(
//             Math::Cos(angle) * radius,
//             Math::Sin(angle) * radius
//         );
//
//         renderer->DrawRect(p, Vector2(4.0f, 4.0f), 0.0f, color, cameraPos, RendererMode::LINES);
//     }
// }
//
// void EnemyDebugDrawer::DrawProximityCircle(Renderer* renderer, const Vector2& position, float radius,
//                                            const Vector2& cameraPos)
// {
//     Vector3 proximityColor = Vector3(0.8f, 0.8f, 1.0f); // Light blue
//     const int segments = 24;
//     const float angleStep = Math::TwoPi / segments;
//
//     for (int i = 0; i < segments; i++)
//     {
//         float angle = i * angleStep;
//
//         Vector2 p = position + Vector2(
//             Math::Cos(angle) * radius,
//             Math::Sin(angle) * radius
//         );
//
//         renderer->DrawRect(p, Vector2(3.0f, 3.0f), 0.0f, proximityColor, cameraPos, RendererMode::LINES);
//     }
// }
//
// void EnemyDebugDrawer::DrawAttackCircle(Renderer* renderer, const Vector2& position, float radius,
//                                         const Vector2& cameraPos)
// {
//     Vector3 attackColor = Vector3(1.0f, 0.2f, 0.2f); // Bright red
//     const int segments = 20;
//     const float angleStep = Math::TwoPi / segments;
//
//     for (int i = 0; i < segments; i++)
//     {
//         float angle = i * angleStep;
//
//         Vector2 p = position + Vector2(
//             Math::Cos(angle) * radius,
//             Math::Sin(angle) * radius
//         );
//
//         // Draw larger points for attack range to make it more visible
//         renderer->DrawRect(p, Vector2(5.0f, 5.0f), 0.0f, attackColor, cameraPos, RendererMode::LINES);
//     }
// }
//
// void EnemyDebugDrawer::DrawPatrolWaypoints(Renderer* renderer, const Vector2 waypoints[2],
//                                            int currentWaypoint, bool isPatrolling, const Vector2& cameraPos)
// {
//     Vector3 waypointColor = Vector3(0.0f, 1.0f, 0.0f); // Green
//     Vector3 currentWaypointColor = Vector3(0.0f, 1.0f, 1.0f); // Cyan for current target
//
//     // Draw waypoint markers
//     for (int i = 0; i < 2; i++)
//     {
//         Vector3 color = (i == currentWaypoint && isPatrolling) ? currentWaypointColor : waypointColor;
//
//         // Draw a square at each waypoint
//         for (int dx = -8; dx <= 8; dx += 4)
//         {
//             for (int dy = -8; dy <= 8; dy += 4)
//             {
//                 renderer->DrawRect(
//                     waypoints[i] + Vector2(dx, dy),
//                     Vector2(4.0f, 4.0f),
//                     0.0f,
//                     color,
//                     cameraPos,
//                     RendererMode::LINES
//                 );
//             }
//         }
//     }
//
//     // Draw line between waypoints
//     Vector2 waypointDiff = waypoints[1] - waypoints[0];
//     const int linePoints = 15;
//     for (int i = 0; i <= linePoints; i++)
//     {
//         float t = i / (float)linePoints;
//         Vector2 point = waypoints[0] + waypointDiff * t;
//         renderer->DrawRect(point, Vector2(2.0f, 2.0f), 0.0f, waypointColor, cameraPos, RendererMode::LINES);
//     }
// }
//
// void EnemyDebugDrawer::DrawLastKnownPosition(Renderer* renderer, const Vector2& lastKnownPos,
//                                              const Vector2& enemyPos, const Vector2& cameraPos)
// {
//     Vector3 markerColor = Vector3(1.0f, 0.0f, 1.0f); // Magenta
//
//     // Draw X at last known position
//     for (int i = -1; i <= 1; i++)
//     {
//         renderer->DrawRect(
//             lastKnownPos + Vector2(i * 4.0f, i * 4.0f),
//             Vector2(5.0f, 5.0f),
//             0.0f,
//             markerColor,
//             cameraPos,
//             RendererMode::LINES
//         );
//         renderer->DrawRect(
//             lastKnownPos + Vector2(i * 4.0f, -i * 4.0f),
//             Vector2(5.0f, 5.0f),
//             0.0f,
//             markerColor,
//             cameraPos,
//             RendererMode::LINES
//         );
//     }
//
//     // Draw line from enemy to last known position
//     Vector2 toLastKnown = lastKnownPos - enemyPos;
//     const int linePoints = 10;
//
//     for (int i = 0; i <= linePoints; i++)
//     {
//         float t = i / (float)linePoints;
//         Vector2 point = enemyPos + toLastKnown * t;
//         renderer->DrawRect(point, Vector2(3.0f, 3.0f), 0.0f, markerColor, cameraPos, RendererMode::LINES);
//     }
// }
