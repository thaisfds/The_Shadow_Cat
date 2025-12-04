#pragma once
#include "Character.h"
#include "../../Components/Physics/ColliderComponent.h"
#include "../../Components/Skills/BasicAttack.h"

class ShadowCat;  // Forward declaration

class Enemy : public Character
{
public:
    enum class EnemyType
    {
        WhiteCat,
        OrangeCat,
        SylvesterCat
    };

    enum class AIState
    {
        Patrol,
        Chase,
        Searching,
        ReturningToPatrol,
        Attack
    };

    Enemy(class Game* game, Vector2 patrolPointA, Vector2 patrolPointB, EnemyType type = EnemyType::WhiteCat, float forwardSpeed = 0.0f);
    virtual ~Enemy();

    void OnUpdate(float deltaTime) override;
    void TakeDamage(int damage) override;
    void Kill() override;
    
    void OnDebugDraw(class Renderer* renderer);
    
    // Getters for debug visualization
    AIState GetCurrentState() const { return mCurrentState; }
    const Vector2* GetPatrolWaypoints() const { return mPatrolWaypoints; }
    int GetCurrentWaypoint() const { return mCurrentWaypoint; }
    float GetDetectionRadius() const { return mDetectionRadius; }
    float GetDetectionAngle() const { return mDetectionAngle; }
    float GetChaseDetectionRadius() const { return mChaseDetectionRadius; }
    float GetProximityRadius() const { return mProximityRadius; }
    float GetAttackRange() const { return mAttackRange; }
    Vector2 GetLastKnownPlayerPos() const { return mLastKnownPlayerPos; }
    bool IsPlayerDetected() const { return mPlayerDetected; }
    Vector2 GetForwardDirection() const;

private:
    CollisionFilter mSkillFilter;
    BasicAttack* mBasicAttack;

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
    float mAttackCooldown;  // Unused: BasicAttack manages cooldown internally
    float mAttackTimer;  // Unused: BasicAttack manages timer internally
    int mAttackDamage;  // Unused: BasicAttack manages damage internally
    
    // Player detection
    float mDetectionRadius;
    float mDetectionAngle;  // Cone angle in radians
    float mChaseDetectionRadius;  // Larger circle used during chase
    float mProximityRadius;  // Small circle for close-range detection (360 degrees)
    bool mPlayerDetected;
    
    // Detection helpers
    const ShadowCat* GetPlayerIfValid() const;
    float GetSquaredDistanceToPlayer(const ShadowCat* player) const;
    bool IsPlayerInRange(float radius) const;  // Generic 360° range detection
    bool IsPlayerInVisionCone() const;  // Directional cone detection (patrol)
    bool IsPlayerInCloseRange() const;  // 360° proximity detection
    bool IsPlayerInChaseRadius() const;  // 360° extended chase detection
    bool IsPlayerInAttackRange() const;  // 360° attack range detection
    void UpdateFacing(const Vector2& direction);
    void MoveToward(const Vector2& target, float speed);
    void UpdatePatrol(float deltaTime);
    void UpdateChase(float deltaTime);
    void UpdateSearching(float deltaTime);
    void UpdateReturningToPatrol(float deltaTime);
    void UpdateAttack(float deltaTime);
};
