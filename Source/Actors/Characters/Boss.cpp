#include "Boss.h"
#include "ShadowCat.h"
#include "../../Game.h"
#include "../../GameConstants.h"
#include "../../Components/Drawing/AnimatorComponent.h"
#include "../../Components/Physics/ColliderComponent.h"
#include "../../Components/Physics/RigidBodyComponent.h"
#include "../../Renderer/Renderer.h"
#include "../../Debug/BossDebugDrawer.h"
#include <cfloat>
#include <SDL.h>

namespace {
    constexpr float SPAWN_ANIMATION_DURATION = 2.0f;
    constexpr float DEATH_ANIMATION_DURATION = 1.0f;
    constexpr int BOSS_BASE_HP = 300;
}

Boss::Boss(class Game* game, Vector2 arenaCenter, BossType type, bool playSpawnAnimation)
    : Character(game, 0.0f)  // Bosses don't use forward speed
    , mDeathTimer(0.0f)
    , mSpawnTimer(0.0f)
    , mBossType(type)
    , mMaxHP(BOSS_BASE_HP)
    , mCurrentState(playSpawnAnimation ? BossState::Spawning : BossState::Idle)
    , mArenaCenter(arenaCenter)
    , mArenaRadius(300.0f)     // Can move up to 300 pixels from arena center
    , mDetectionRadius(350.0f) // Detect player in large arena area
    , mAttackRange(120.0f)     // Melee attack range (increased for boss)
{
    // Set position to arena center
    SetPosition(arenaCenter);
    
    // Select sprite based on boss type
    const char* spriteName = "WhiteBossAnim";
    switch (type)
    {
        case BossType::OrangeBoss:
            spriteName = "OrangeBossAnim";
            break;
        case BossType::WhiteBoss:
        default:
            spriteName = "WhiteBossAnim";
            break;
    }
    
    // Boss sprites are 64x64 (double the tile size)
    // Scale to 128x128 for proper boss size (2x regular enemies)
    mAnimatorComponent = new AnimatorComponent(this, spriteName, 128, 128);
    mRigidBodyComponent = new RigidBodyComponent(this);
    
    // Larger collider for boss: width=112, height=96 (scaled to match 128x128 sprite)
    Collider* collider = new AABBCollider(112, 96);
    mColliderComponent = new ColliderComponent(this, 0, 16, collider, GetBaseEnemyFilter());
    
    // Disable gravity for boss (stationary arena defender)
    mRigidBodyComponent->SetApplyGravity(false);

    // Set boss health
    hp = mMaxHP;

    // Setup animations based on sprite data
    // WhiteBoss: Idle (0), Walk (1-2), Attack (3-4)
    // OrangeBoss: Idle (0), Walk (1-2), BasicAttack (3-4), SpecialAttack (5-6)
    mAnimatorComponent->AddAnimation("Idle", {0});
    mAnimatorComponent->AddAnimation("Walk", {1, 2});
    mAnimatorComponent->AddAnimation("Run", {1, 2});  // Alias for Walk
    mAnimatorComponent->AddAnimation("Attack", {3, 4});
    mAnimatorComponent->AddAnimation("BasicAttack", {3, 4});  // Alias for BasicAttack skill
    mAnimatorComponent->AddAnimation("Hit", {0, 3});
    mAnimatorComponent->AddAnimation("Death", {4, 3, 2, 1, 0});
    
    // Start with appropriate animation
    if (playSpawnAnimation)
    {
        mAnimatorComponent->LoopAnimation("Idle");  // Can be customized for spawn
    }
    else
    {
        mAnimatorComponent->LoopAnimation("Idle");
    }

    // Setup attack skill
    mSkillFilter.belongsTo = CollisionFilter::GroupMask({CollisionGroup::EnemySkills});
    mSkillFilter.collidesWith = CollisionFilter::GroupMask({CollisionGroup::Player});
    // Boss attack: 2 damage, default update order (100), 200px cone radius, 100 degree cone angle
    mBasicAttack = new BasicAttack(this, mSkillFilter, 2, 100, 200.0f, 100.0f);
}

Boss::~Boss()
{
}

void Boss::OnUpdate(float deltaTime)
{
    Character::OnUpdate(deltaTime);
    
    // Update attack skill
    if (mBasicAttack)
    {
        mBasicAttack->Update(deltaTime);
    }

    // Handle death state
    if (mCurrentState == BossState::Dead)
    {
        mDeathTimer += deltaTime;
        
        if (mDeathTimer >= DEATH_ANIMATION_DURATION)
        {
            SetState(ActorState::Destroy);
        }
        return;
    }
    
    // Don't process AI if dead
    if (mIsDead) return;
    
    // Check player detection
    bool playerInArena = IsPlayerInArena();
    bool playerInAttackRange = IsPlayerInAttackRange();
    
    // State transitions
    switch (mCurrentState)
    {
        case BossState::Spawning:
            // Spawning completes after animation duration
            // Transition handled in UpdateSpawning
            break;
            
        case BossState::Idle:
            if (playerInArena)
            {
                mCurrentState = BossState::Combat;
                if (mGame->IsDebugging()) SDL_Log("Boss: Idle -> Combat");
            }
            break;
            
        case BossState::Combat:
            if (playerInAttackRange)
            {
                mCurrentState = BossState::Attacking;
                if (mGame->IsDebugging()) SDL_Log("Boss: Combat -> Attacking");
            }
            else if (!playerInArena)
            {
                mCurrentState = BossState::Idle;
                if (mGame->IsDebugging()) SDL_Log("Boss: Combat -> Idle (player left arena)");
            }
            break;
            
        case BossState::Attacking:
            if (!playerInAttackRange)
            {
                mCurrentState = BossState::Combat;
                if (mGame->IsDebugging()) SDL_Log("Boss: Attacking -> Combat");
            }
            break;
            
        case BossState::Dead:
            // Already handled above
            break;
    }
    
    // Execute state behavior
    switch (mCurrentState)
    {
        case BossState::Spawning:
            UpdateSpawning(deltaTime);
            break;
        case BossState::Idle:
            UpdateIdle(deltaTime);
            break;
        case BossState::Combat:
            UpdateCombat(deltaTime);
            break;
        case BossState::Attacking:
            UpdateAttacking(deltaTime);
            break;
        case BossState::Dead:
            // Already handled above
            break;
    }
}

void Boss::TakeDamage(int damage)
{
    if (mIsDead) return;
    
    // Cannot take damage while spawning
    if (mCurrentState == BossState::Spawning) return;

    hp = Math::Max(hp - damage, 0);
    
    if (hp <= 0)
    {
        Kill();
    }
    else
    {
        mAnimatorComponent->PlayAnimationOnce("Hit");
    }
}

void Boss::Kill()
{
    if (mIsDead) return;

    mIsDead = true;
    mCurrentState = BossState::Dead;
    mDeathTimer = 0.0f;

    // Stop movement
    mRigidBodyComponent->SetVelocity(Vector2::Zero);
    
    // Lock animations and movement
    SetAnimationLock(true);
    SetMovementLock(true);

    // Disable collider
    if (mColliderComponent)
    {
        mColliderComponent->SetEnabled(false);
    }

    // Play death animation
    mAnimatorComponent->PlayAnimation("Death", 1);
    
    if (mGame->IsDebugging())
    {
        SDL_Log("Boss: Defeated!");
    }
}

// Helper: Get player reference if valid
const ShadowCat* Boss::GetPlayerIfValid() const
{
    return mGame->GetPlayer();
}

// Helper: Calculate squared distance to player
float Boss::GetSquaredDistanceToPlayer(const ShadowCat* player) const
{
    if (!player) return FLT_MAX;
    Vector2 playerOffset = player->GetPosition() - mPosition;
    return playerOffset.LengthSq();
}

// Check if player is within arena detection radius
bool Boss::IsPlayerInArena() const
{
    const ShadowCat* player = GetPlayerIfValid();
    if (!player) return false;
    
    float radiusSquared = mDetectionRadius * mDetectionRadius;
    return GetSquaredDistanceToPlayer(player) <= radiusSquared;
}

// Check if player is within attack range
bool Boss::IsPlayerInAttackRange() const
{
    const ShadowCat* player = GetPlayerIfValid();
    if (!player) return false;
    
    float rangeSquared = mAttackRange * mAttackRange;
    return GetSquaredDistanceToPlayer(player) <= rangeSquared;
}

void Boss::UpdateFacing(const Vector2& direction)
{
    if (direction.x > 0.0f)
        SetScale(Vector2(1.0f, 1.0f));
    else if (direction.x < 0.0f)
        SetScale(Vector2(-1.0f, 1.0f));
}

void Boss::UpdateSpawning(float deltaTime)
{
    mSpawnTimer += deltaTime;
    
    // Stop movement during spawn
    mRigidBodyComponent->SetVelocity(Vector2::Zero);
    mIsMoving = false;
    
    // Transition to Idle after spawn animation completes
    if (mSpawnTimer >= SPAWN_ANIMATION_DURATION)
    {
        mCurrentState = BossState::Idle;
        if (mGame->IsDebugging())
        {
            SDL_Log("Boss: Spawning complete -> Idle");
        }
    }
}

void Boss::UpdateIdle(float deltaTime)
{
    // Boss stands still, waiting for player
    mRigidBodyComponent->SetVelocity(Vector2::Zero);
    mIsMoving = false;
    
    // Face player if they're nearby
    const ShadowCat* player = GetPlayerIfValid();
    if (player)
    {
        Vector2 toPlayer = player->GetPosition() - mPosition;
        UpdateFacing(toPlayer);
    }
}

void Boss::UpdateCombat(float deltaTime)
{
    const ShadowCat* player = GetPlayerIfValid();
    if (!player) return;
    
    Vector2 toPlayer = player->GetPosition() - mPosition;
    float distanceToPlayer = toPlayer.Length();
    
    // Move toward player if not in attack range
    if (distanceToPlayer > mAttackRange)
    {
        toPlayer.Normalize();
        Vector2 velocity = toPlayer * 40.0f;  // Slow movement speed (boss is heavy)
        mRigidBodyComponent->SetVelocity(velocity);
        mIsMoving = true;  // Play Walk animation
        UpdateFacing(toPlayer);
    }
    else
    {
        // Stop when in attack range
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
        mIsMoving = false;
        UpdateFacing(toPlayer);
    }
}

void Boss::UpdateAttacking(float deltaTime)
{
    const ShadowCat* player = GetPlayerIfValid();
    if (!player) return;

    // Stop moving during attack
    mRigidBodyComponent->SetVelocity(Vector2::Zero);
    mIsMoving = false;
    
    // Face the player
    Vector2 toPlayer = player->GetPosition() - mPosition;
    UpdateFacing(toPlayer);

    // Execute attack if ready
    if (mBasicAttack->CanUse())
    {
        mBasicAttack->Execute(player->GetPosition());
    }
}

void Boss::OnDebugDraw(class Renderer* renderer)
{
    BossDebugDrawer::Draw(renderer, this, mGame);
}
