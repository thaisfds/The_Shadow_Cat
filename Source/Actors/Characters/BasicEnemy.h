#pragma once
#include "Character.h"

class BasicEnemy : public Character
{
public:
    enum class AIState
    {
        Patrol,
        Chase
    };

    BasicEnemy(class Game* game, float forwardSpeed = 0.0f, float patrolDistance = 200.0f);
    virtual ~BasicEnemy();

    void OnUpdate(float deltaTime) override;
    void TakeDamage(int damage) override;
    void Kill() override;
    
    void OnDebugDraw(class Renderer* renderer);

private:
    float mDeathTimer;
    bool mIsPlayingDeathAnim;
    
    // AI State
    AIState mCurrentState;
    
    // Patrol movement
    Vector2 mPatrolStartPos;
    Vector2 mPreviousPosition;
    float mPatrolDistance;
    int mPatrolDirection;  // 1 for right, -1 for left
    float mPatrolSpeed;
    
    // Chase behavior
    float mChaseSpeed;
    
    // Player detection
    float mDetectionRadius;
    bool mPlayerDetected;
    
    bool IsPlayerInRange() const;
    void UpdatePatrol(float deltaTime);
    void UpdateChase(float deltaTime);
};
