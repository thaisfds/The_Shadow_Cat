#pragma once
#include "Character.h"

class Enemy : public Character
{
public:
    enum class AIState
    {
        Patrol,
        Chase,
        Searching,
        ReturningToPatrol,
        Attack
    };

    Enemy(class Game* game, Vector2 patrolPointA, Vector2 patrolPointB, float forwardSpeed = 0.0f);
    virtual ~Enemy();

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
    Vector2 mPatrolWaypoints[2];  // Two patrol points to walk between
    int mCurrentWaypoint;  // Index of current target waypoint (0 or 1)
    Vector2 mPreviousPosition;
    float mPatrolSpeed;
    Vector2 mMovementDirection;  // Actual movement direction for cone detection
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
    void UpdateReturningToPatrol(float deltaTime);
    void UpdateAttack(float deltaTime);
};
