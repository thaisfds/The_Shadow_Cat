#pragma once
#include "../Math.h"

class Renderer;
class Boss;
class Game;

class BossDebugDrawer
{
public:
    // Main entry point for drawing all boss debug visualization
    static void Draw(Renderer* renderer, const Boss* boss, Game* game);

private:
    // State-specific visualization methods
    static void DrawSpawningState(Renderer* renderer, const Boss* boss, Game* game);
    static void DrawIdleState(Renderer* renderer, const Boss* boss, Game* game);
    static void DrawCombatState(Renderer* renderer, const Boss* boss, Game* game);
    static void DrawAttackingState(Renderer* renderer, const Boss* boss, Game* game);
    
    // Shared drawing primitives
    static void DrawCircle(Renderer* renderer, const Vector2& position, float radius, 
                           const Vector3& color, const Vector2& cameraPos, int segments = 32);
    static void DrawArenaRadius(Renderer* renderer, const Vector2& center, float radius, 
                                const Vector2& cameraPos);
    static void DrawDetectionRadius(Renderer* renderer, const Vector2& position, float radius, 
                                    bool playerDetected, const Vector2& cameraPos);
    static void DrawAttackRange(Renderer* renderer, const Vector2& position, float radius, 
                                const Vector2& cameraPos);
    static void DrawLineToPlayer(Renderer* renderer, const Vector2& bossPos, 
                                 const Vector2& playerPos, float distance, const Vector2& cameraPos);
};
