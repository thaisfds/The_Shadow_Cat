#include "BossDebugDrawer.h"
#include "../Actors/Characters/Boss.h"
#include "../Actors/Characters/ShadowCat.h"
#include "../Renderer/Renderer.h"
#include "../Game.h"
#include <SDL.h>

void BossDebugDrawer::Draw(Renderer* renderer, const Boss* boss, Game* game)
{
    if (!game->IsDebugging()) return;
    
    Vector2 position = boss->GetPosition();
    Vector2 cameraPos = game->GetCameraPos();
    
    // Draw line to player if player exists
    auto player = game->GetPlayer();
    if (player && boss->GetCurrentState() != Boss::BossState::Dead)
    {
        Vector2 playerPos = player->GetPosition();
        float distance = (playerPos - position).Length();
        DrawLineToPlayer(renderer, position, playerPos, distance, cameraPos);
    }
    
    // Draw state-specific visualization
    switch (boss->GetCurrentState())
    {
        case Boss::BossState::Spawning:
            DrawSpawningState(renderer, boss, game);
            break;
            
        case Boss::BossState::Idle:
            DrawIdleState(renderer, boss, game);
            break;
            
        case Boss::BossState::Combat:
            DrawCombatState(renderer, boss, game);
            break;
            
        case Boss::BossState::Attacking:
            DrawAttackingState(renderer, boss, game);
            break;
            
        case Boss::BossState::Dead:
            // No debug draw for dead state
            break;
    }
    
    // Always draw arena center marker
    Vector2 arenaCenter = boss->GetArenaCenter();
    Vector3 centerColor = Vector3(1.0f, 1.0f, 1.0f); // White
    
    // Draw crosshair at arena center
    for (int i = -12; i <= 12; i += 4)
    {
        renderer->DrawRect(arenaCenter + Vector2(i, 0), Vector2(4.0f, 4.0f), 0.0f, centerColor, cameraPos, RendererMode::LINES);
        renderer->DrawRect(arenaCenter + Vector2(0, i), Vector2(4.0f, 4.0f), 0.0f, centerColor, cameraPos, RendererMode::LINES);
    }
}

void BossDebugDrawer::DrawSpawningState(Renderer* renderer, const Boss* boss, Game* game)
{
    Vector2 position = boss->GetPosition();
    Vector2 cameraPos = game->GetCameraPos();
    Vector3 spawnColor = Vector3(1.0f, 1.0f, 0.0f); // Yellow
    
    // Draw pulsing circle during spawn
    DrawCircle(renderer, position, 100.0f, spawnColor, cameraPos, 16);
}

void BossDebugDrawer::DrawIdleState(Renderer* renderer, const Boss* boss, Game* game)
{
    Vector2 position = boss->GetPosition();
    Vector2 arenaCenter = boss->GetArenaCenter();
    Vector2 cameraPos = game->GetCameraPos();
    
    // Draw detection radius (arena range)
    DrawDetectionRadius(renderer, position, boss->GetDetectionRadius(), false, cameraPos);
    
    // Draw arena radius
    DrawArenaRadius(renderer, arenaCenter, 300.0f, cameraPos);
}

void BossDebugDrawer::DrawCombatState(Renderer* renderer, const Boss* boss, Game* game)
{
    Vector2 position = boss->GetPosition();
    Vector2 arenaCenter = boss->GetArenaCenter();
    Vector2 cameraPos = game->GetCameraPos();
    
    // Draw detection radius (player detected - red)
    DrawDetectionRadius(renderer, position, boss->GetDetectionRadius(), true, cameraPos);
    
    // Draw attack range
    DrawAttackRange(renderer, position, boss->GetAttackRange(), cameraPos);
    
    // Draw arena radius
    DrawArenaRadius(renderer, arenaCenter, 300.0f, cameraPos);
}

void BossDebugDrawer::DrawAttackingState(Renderer* renderer, const Boss* boss, Game* game)
{
    Vector2 position = boss->GetPosition();
    Vector2 arenaCenter = boss->GetArenaCenter();
    Vector2 cameraPos = game->GetCameraPos();
    
    // Draw attack range (bright red during attack)
    DrawAttackRange(renderer, position, boss->GetAttackRange(), cameraPos);
    
    // Draw detection radius
    DrawDetectionRadius(renderer, position, boss->GetDetectionRadius(), true, cameraPos);
    
    // Draw arena radius
    DrawArenaRadius(renderer, arenaCenter, 300.0f, cameraPos);
}

void BossDebugDrawer::DrawCircle(Renderer* renderer, const Vector2& position, float radius,
                                  const Vector3& color, const Vector2& cameraPos, int segments)
{
    const float angleStep = Math::TwoPi / segments;
    
    for (int i = 0; i < segments; i++)
    {
        float angle = i * angleStep;
        
        Vector2 p = position + Vector2(
            Math::Cos(angle) * radius,
            Math::Sin(angle) * radius
        );
        
        renderer->DrawRect(p, Vector2(4.0f, 4.0f), 0.0f, color, cameraPos, RendererMode::LINES);
    }
}

void BossDebugDrawer::DrawArenaRadius(Renderer* renderer, const Vector2& center, float radius,
                                      const Vector2& cameraPos)
{
    Vector3 arenaColor = Vector3(0.5f, 0.5f, 1.0f); // Light blue
    const int segments = 48; // More segments for smoother circle
    const float angleStep = Math::TwoPi / segments;
    
    for (int i = 0; i < segments; i++)
    {
        float angle = i * angleStep;
        
        Vector2 p = center + Vector2(
            Math::Cos(angle) * radius,
            Math::Sin(angle) * radius
        );
        
        renderer->DrawRect(p, Vector2(3.0f, 3.0f), 0.0f, arenaColor, cameraPos, RendererMode::LINES);
    }
}

void BossDebugDrawer::DrawDetectionRadius(Renderer* renderer, const Vector2& position, float radius,
                                          bool playerDetected, const Vector2& cameraPos)
{
    Vector3 color = playerDetected ? Vector3(1.0f, 0.0f, 0.0f) : Vector3(0.0f, 1.0f, 0.0f); // Red if detected, green otherwise
    const int segments = 40;
    const float angleStep = Math::TwoPi / segments;
    
    for (int i = 0; i < segments; i++)
    {
        float angle = i * angleStep;
        
        Vector2 p = position + Vector2(
            Math::Cos(angle) * radius,
            Math::Sin(angle) * radius
        );
        
        renderer->DrawRect(p, Vector2(4.0f, 4.0f), 0.0f, color, cameraPos, RendererMode::LINES);
    }
}

void BossDebugDrawer::DrawAttackRange(Renderer* renderer, const Vector2& position, float radius,
                                      const Vector2& cameraPos)
{
    Vector3 attackColor = Vector3(1.0f, 0.2f, 0.2f); // Bright red
    const int segments = 24;
    const float angleStep = Math::TwoPi / segments;
    
    for (int i = 0; i < segments; i++)
    {
        float angle = i * angleStep;
        
        Vector2 p = position + Vector2(
            Math::Cos(angle) * radius,
            Math::Sin(angle) * radius
        );
        
        // Draw larger points for attack range visibility
        renderer->DrawRect(p, Vector2(6.0f, 6.0f), 0.0f, attackColor, cameraPos, RendererMode::LINES);
    }
}

void BossDebugDrawer::DrawLineToPlayer(Renderer* renderer, const Vector2& bossPos,
                                        const Vector2& playerPos, float distance, const Vector2& cameraPos)
{
    Vector3 lineColor;
    
    // Color based on distance ranges
    if (distance <= 120.0f)  // Attack range
        lineColor = Vector3(1.0f, 0.0f, 0.0f);  // Red
    else if (distance <= 350.0f)  // Detection range
        lineColor = Vector3(1.0f, 0.5f, 0.0f);  // Orange
    else
        lineColor = Vector3(0.5f, 0.5f, 0.5f);  // Gray
    
    // Draw line from boss to player
    Vector2 toPlayer = playerPos - bossPos;
    const int linePoints = 15;
    
    for (int i = 0; i <= linePoints; i++)
    {
        float t = i / (float)linePoints;
        Vector2 point = bossPos + toPlayer * t;
        renderer->DrawRect(point, Vector2(3.0f, 3.0f), 0.0f, lineColor, cameraPos, RendererMode::LINES);
    }
    
    // Draw distance text at midpoint (represented as small markers)
    Vector2 midpoint = (bossPos + playerPos) * 0.5f;
    
    // Draw a marker at midpoint
    for (int dx = -6; dx <= 6; dx += 3)
    {
        renderer->DrawRect(midpoint + Vector2(dx, 0), Vector2(4.0f, 4.0f), 0.0f, lineColor, cameraPos, RendererMode::LINES);
        renderer->DrawRect(midpoint + Vector2(0, dx), Vector2(4.0f, 4.0f), 0.0f, lineColor, cameraPos, RendererMode::LINES);
    }
}
