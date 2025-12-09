#pragma once
#include "Character.h"
#include "../../Components/Physics/ColliderComponent.h"
#include "../../Components/Skills/BasicAttack.h"

class ShadowCat;  // Forward declaration

class Boss : public Character
{
public:
    enum class BossType
    {
        WhiteBoss,
        OrangeBoss
    };

    enum class BossState
    {
        Spawning,   // Optional intro animation
        Idle,       // Waiting for player to enter arena
        Combat,     // Player in arena, actively fighting
        Attacking,  // Executing attack
        Dead        // Death animation playing
    };

    Boss(class Game* game, Vector2 arenaCenter, BossType type = BossType::WhiteBoss, 
         bool playSpawnAnimation = false);
    virtual ~Boss();

    void OnUpdate(float deltaTime) override;
    void TakeDamage(int damage) override;
    void Kill() override;
    
    void OnDebugDraw(class Renderer* renderer);
    
    // Getters
    BossState GetCurrentState() const { return mCurrentState; }
    Vector2 GetArenaCenter() const { return mArenaCenter; }
    float GetDetectionRadius() const { return mDetectionRadius; }
    float GetAttackRange() const { return mAttackRange; }
    int GetMaxHP() const { return mMaxHP; }

private:
    CollisionFilter mSkillFilter;
    BasicAttack* mBasicAttack;

    float mDeathTimer;
    float mSpawnTimer;
    
    // Boss identity
    BossType mBossType;
    int mMaxHP;
    
    // AI State
    BossState mCurrentState;
    
    // Arena behavior
    Vector2 mArenaCenter;      // Boss defends this position
    float mArenaRadius;        // How far boss can move from center
    
    // Combat parameters
    float mDetectionRadius;    // 360° detection range
    float mAttackRange;        // 360° attack range
    
    // Helper methods
    const ShadowCat* GetPlayerIfValid() const;
    float GetSquaredDistanceToPlayer(const ShadowCat* player) const;
    bool IsPlayerInArena() const;
    bool IsPlayerInAttackRange() const;
    void UpdateFacing(const Vector2& direction);
    
    // State updates
    void UpdateSpawning(float deltaTime);
    void UpdateIdle(float deltaTime);
    void UpdateCombat(float deltaTime);
    void UpdateAttacking(float deltaTime);
};
