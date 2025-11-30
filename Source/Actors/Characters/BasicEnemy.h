#pragma once
#include "Character.h"

class BasicEnemy : public Character
{
public:
    enum class AIState
    {
        Patrol,
        Chase,
        Searching,
        Attack
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
    bool mIsPatrolPaused;  // Pausing before turning
    float mPatrolPauseTimer;
    float mPatrolPauseDuration;  // How long to pause before turning
    
    // Chase behavior
    float mChaseSpeed;
    
    // Searching behavior
    Vector2 mLastKnownPlayerPos;
    float mSearchTimer;
    float mSearchDuration;  // How long to search before giving up
    float mSearchSpeed;
    
    // Attack behavior
    float mAttackRange;
    float mAttackCooldown;
    float mAttackTimer;
    int mAttackDamage;
    
    // Player detection
    float mDetectionRadius;
    float mDetectionAngle;  // Cone angle in radians
    float mChaseDetectionRadius;  // Larger circle used during chase
    float mProximityRadius;  // Small circle for close-range detection (360 degrees)
    bool mPlayerDetected;
    
    bool IsPlayerInRange() const;
    bool IsPlayerInProximity() const;
    bool IsPlayerInChaseRange() const;
    bool IsPlayerInAttackRange() const;
    Vector2 GetForwardDirection() const;
    void UpdatePatrol(float deltaTime);
    void UpdateChase(float deltaTime);
    void UpdateSearching(float deltaTime);
    void UpdateAttack(float deltaTime);
};
