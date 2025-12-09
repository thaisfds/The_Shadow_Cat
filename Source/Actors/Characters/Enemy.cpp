#include "Enemy.h"
#include "ShadowCat.h"
#include "../../Game.h"
#include "../../GameConstants.h"
#include "../../Components/Drawing/AnimatorComponent.h"
#include "../../Components/Physics/ColliderComponent.h"
#include "../../Components/Physics/RigidBodyComponent.h"
#include "../../Renderer/Renderer.h"
#include "../../Debug/EnemyDebugDrawer.h"
#include <cfloat>
#include <SDL.h>

#include "../../Components/Skills/ClawAttack.h"
#include "../../Components/Skills/Dash.h"
#include "../../Components/Skills/ShadowForm.h"

namespace {
    constexpr float WAYPOINT_REACHED_DISTANCE = 10.0f;
    constexpr float PATROL_AREA_REACHED_DISTANCE = 20.0f;
    constexpr float LAST_KNOWN_POSITION_REACHED_DISTANCE = 20.0f;
    constexpr float MIN_POSITION_CHANGE_TO_DETECT_STUCK = 0.1f;
    constexpr float DEATH_ANIMATION_DURATION = 0.5f;
}

Enemy::Enemy(class Game* game, Vector2 patrolPointA, Vector2 patrolPointB, EnemyType type, float forwardSpeed)
    : Character(game, forwardSpeed)
    , mDeathTimer(0.0f)
    , mIsPlayingDeathAnim(false)
    , mCurrentState(AIState::Patrol)
    , mCurrentWaypoint(0)  // Start by moving toward first waypoint
    , mPreviousPosition(Vector2::Zero)
    , mPatrolSpeed(50.0f)
    , mMovementDirection(Vector2(1.0f, 0.0f))  // Start facing right
    , mIsPatrolPaused(false)
    , mPatrolPauseTimer(0.0f)
    , mPatrolPauseDuration(0.75f)  // Pause for 0.75 seconds before turning
    , mChaseSpeed(80.0f)
    , mLastKnownPlayerPos(Vector2::Zero)
    , mSearchTimer(0.0f)
    , mSearchDuration(5.0f)  // Search for 5 seconds before giving up
    , mSearchSpeed(60.0f)  // Slower than chase speed
    , mAttackRange(64.0f)
    , mAttackCooldown(1.5f)  // NOTE: Unused - BasicAttack manages cooldown internally
    , mAttackTimer(0.0f)  // NOTE: Unused - BasicAttack manages timer internally
    , mAttackDamage(1)  // NOTE: Unused - BasicAttack manages damage internally
    , mDetectionRadius(200.0f)
    , mDetectionAngle(Math::ToRadians(90.0f))  // 90 degree cone in front
    , mChaseDetectionRadius(250.0f)  // 25% larger radius for chase persistence
    , mProximityRadius(100.0f)  // Close-range detection (about 1.6 tiles)
    , mPlayerDetected(false)
{
    // Store patrol waypoints
    mPatrolWaypoints[0] = patrolPointA;
    mPatrolWaypoints[1] = patrolPointB;
    
    // Select sprite based on enemy type
    const char* animName = "WhiteCatAnim";
    switch (type)
    {
        case EnemyType::OrangeCat:
            animName = "OrangeCatAnim";
            break;
        case EnemyType::SylvesterCat:
            animName = "SylvesterCatAnim";
            break;
        case EnemyType::WhiteCat:
        default:
            animName = "WhiteCatAnim";
            break;
    }
    
    mAnimatorComponent = new AnimatorComponent(this, animName, GameConstants::TILE_SIZE, GameConstants::TILE_SIZE);
    mRigidBodyComponent = new RigidBodyComponent(this);
    
    // Collider dimensions: width=48 (slightly smaller than tile), height=32 (half tile), offset=16 for ground alignment
    Collider *collider = new AABBCollider(48, 32);
    mColliderComponent = new ColliderComponent(this, 0, 16, collider, GetBaseEnemyFilter());
    
    // Set enemy health (lower than player's 10)
    hp = 30;

    // Setup animations
    mAnimatorComponent->AddAnimation("Idle", {1});  // Frame 1 is idle (still frame)
    mAnimatorComponent->AddAnimation("Run", {0, 2});  // Frames 0 and 2 for running (animated)
    mAnimatorComponent->AddAnimation("Hit", {0, 1});  // Flicker between frames for hit effect
    mAnimatorComponent->AddAnimation("Death", {2, 1, 0});  // Run animation frames as death effect

    mAnimatorComponent->LoopAnimation("Run");

    mSkillFilter.belongsTo = CollisionFilter::GroupMask({CollisionGroup::EnemySkills});
    mSkillFilter.collidesWith = CollisionFilter::GroupMask({CollisionGroup::Player});
    mSkill = new BasicAttack(this);
    
    // Register with Game for tracking
    game->RegisterEnemy(this);
}

Enemy::~Enemy()
{
    // Unregister from Game
    GetGame()->UnregisterEnemy(this);
}

void Enemy::OnUpdate(float deltaTime)
{
    Character::OnUpdate(deltaTime);

    // Handle death animation and destruction
    if (mIsPlayingDeathAnim)
    {
        mDeathTimer += deltaTime;
        
        if (mDeathTimer >= DEATH_ANIMATION_DURATION)
        {
            SetState(ActorState::Destroy);
        }
        return;
    }
    
    // Don't move if dead
    if (mIsDead) return;
    
    // Check for player detection (early evaluation for all states)
    bool playerInCloseRange = IsPlayerInCloseRange();   // 360° proximity detection
    bool playerInVisionCone = IsPlayerInVisionCone();   // Directional cone detection
    bool playerInChaseRadius = IsPlayerInChaseRadius(); // 360° extended chase radius
    bool playerInAttackRange = IsPlayerInAttackRange(); // 360° attack range
    
    // State transitions - priority order: Attack > Chase > Search > Return > Patrol
    switch (mCurrentState)
    {
        case AIState::Patrol:
            if (playerInAttackRange)
            {
                mCurrentState = AIState::Attack;
                if (mGame->IsDebugging()) SDL_Log("Enemy: Patrol -> Attack");
            }
            else if (playerInCloseRange || playerInVisionCone)
            {
                mCurrentState = AIState::Chase;
                if (mGame->IsDebugging()) SDL_Log("Enemy: Patrol -> Chase (%s)", 
                    playerInCloseRange ? "close range" : "vision cone");
            }
            break;
            
        case AIState::Chase:
            if (playerInAttackRange)
            {
                mCurrentState = AIState::Attack;
                if (mGame->IsDebugging()) SDL_Log("Enemy: Chase -> Attack");
            }
            else if (!playerInChaseRadius)
            {
                mCurrentState = AIState::Searching;
                mSearchTimer = 0.0f;
                if (mGame->IsDebugging()) SDL_Log("Enemy: Chase -> Searching (last known: %.2f, %.2f)", 
                    mLastKnownPlayerPos.x, mLastKnownPlayerPos.y);
            }
            break;
            
        case AIState::Attack:
            if (!playerInAttackRange)
            {
                if (playerInChaseRadius)
                {
                    mCurrentState = AIState::Chase;
                    if (mGame->IsDebugging()) SDL_Log("Enemy: Attack -> Chase");
                }
                else
                {
                    mCurrentState = AIState::Searching;
                    mSearchTimer = 0.0f;
                    if (mGame->IsDebugging()) SDL_Log("Enemy: Attack -> Searching");
                }
            }
            break;
            
        case AIState::Searching:
            // Searching uses vision cone detection (directional)
            if (playerInAttackRange)
            {
                mCurrentState = AIState::Attack;
                if (mGame->IsDebugging()) SDL_Log("Enemy: Searching -> Attack");
            }
            else if (playerInCloseRange || playerInVisionCone)
            {
                mCurrentState = AIState::Chase;
                if (mGame->IsDebugging()) SDL_Log("Enemy: Searching -> Chase");
            }
            break;
            
        case AIState::ReturningToPatrol:
            // ReturningToPatrol uses 360° radius detection (more alert)
            if (playerInAttackRange)
            {
                mCurrentState = AIState::Attack;
                if (mGame->IsDebugging()) SDL_Log("Enemy: Returning -> Attack");
            }
            else if (playerInCloseRange || playerInChaseRadius)
            {
                mCurrentState = AIState::Chase;
                if (mGame->IsDebugging()) SDL_Log("Enemy: Returning -> Chase");
            }
            break;
    }
    
    // Update detected flag for debug visualization (reflects actual detection logic per state)
    switch (mCurrentState)
    {
        case AIState::Patrol:
            mPlayerDetected = playerInCloseRange || playerInVisionCone;
            break;
        case AIState::Chase:
            mPlayerDetected = playerInChaseRadius;
            break;
        case AIState::Attack:
            mPlayerDetected = playerInAttackRange;
            break;
        case AIState::Searching:
            mPlayerDetected = playerInCloseRange || playerInVisionCone;
            break;
        case AIState::ReturningToPatrol:
            mPlayerDetected = playerInCloseRange || playerInChaseRadius;
            break;
    }
    
    // Execute state behavior
    switch (mCurrentState)
    {
        case AIState::Patrol:
            UpdatePatrol(deltaTime);
            break;
        case AIState::Chase:
            UpdateChase(deltaTime);
            break;
        case AIState::Searching:
            UpdateSearching(deltaTime);
            break;
        case AIState::ReturningToPatrol:
            UpdateReturningToPatrol(deltaTime);
            break;
        case AIState::Attack:
            UpdateAttack(deltaTime);
            break;
    }
}

void Enemy::TakeDamage(int damage)
{
    if (mIsDead) return;

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

// Helper: Get player reference if valid
const ShadowCat* Enemy::GetPlayerIfValid() const
{
    return mGame->GetPlayer();
}

// Helper: Calculate squared distance to player (avoids sqrt for performance)
float Enemy::GetSquaredDistanceToPlayer(const ShadowCat* player) const
{
    if (!player) return FLT_MAX;
    Vector2 playerOffset = player->GetPosition() - mPosition;
    return playerOffset.LengthSq();
}

// Helper: Check if player is within a specific radius (360° circular detection)
bool Enemy::IsPlayerInRange(float radius) const
{
    const ShadowCat* player = GetPlayerIfValid();
    if (!player) return false;
    
    float radiusSquared = radius * radius;
    return GetSquaredDistanceToPlayer(player) <= radiusSquared;
}

bool Enemy::IsPlayerInVisionCone() const
{
    const ShadowCat* player = GetPlayerIfValid();
    if (!player) return false;
    
    // Use chase radius during Searching and ReturningToPatrol states, normal detection radius otherwise
    float radius = (mCurrentState == AIState::Searching || mCurrentState == AIState::ReturningToPatrol) 
        ? mChaseDetectionRadius : mDetectionRadius;
    float radiusSquared = radius * radius;
    
    // Check distance first (early exit for performance)
    float distanceSquared = GetSquaredDistanceToPlayer(player);
    if (distanceSquared > radiusSquared)
        return false;
    
    // Check if player is within detection cone (in front of enemy)
    Vector2 toPlayer = player->GetPosition() - mPosition;
    toPlayer.Normalize();
    Vector2 forward = GetForwardDirection();
    
    // Calculate angle between forward direction and direction to player
    float dotProduct = Vector2::Dot(forward, toPlayer);
    float angleToPlayer = Math::Acos(dotProduct);
    
    // Player is detected if within half the cone angle
    return angleToPlayer <= (mDetectionAngle / 2.0f);
}

// 360° proximity detection - close range circular detection (no directional requirement)
bool Enemy::IsPlayerInCloseRange() const
{
    return IsPlayerInRange(mProximityRadius);
}

// 360° extended chase radius - larger circular detection for maintaining chase
bool Enemy::IsPlayerInChaseRadius() const
{
    return IsPlayerInRange(mChaseDetectionRadius);
}

Vector2 Enemy::GetForwardDirection() const
{
    // Use actual movement direction if available (length > 0.1)
    // This ensures cone follows movement direction in Patrol and Searching states
    if (mMovementDirection.LengthSq() > 0.01f)
    {
        Vector2 dir = mMovementDirection;
        dir.Normalize();
        return dir;
    }
    
    // Fallback to sprite facing direction when stationary
    // Enemy faces right when scale.x > 0, left when scale.x < 0
    if (mScale.x > 0.0f)
        return Vector2(1.0f, 0.0f);  // Facing right
    else
        return Vector2(-1.0f, 0.0f);  // Facing left
}

// 360° attack range detection - checks if player is close enough to attack
bool Enemy::IsPlayerInAttackRange() const
{
    return IsPlayerInRange(mAttackRange);
}

void Enemy::UpdateFacing(const Vector2& direction)
{
    if (direction.x > 0.0f)
        SetScale(Vector2(1.0f, 1.0f));
    else if (direction.x < 0.0f)
        SetScale(Vector2(-1.0f, 1.0f));
}

void Enemy::MoveToward(const Vector2& target, float speed)
{
    if (mIsMovementLocked) return;
    
    Vector2 direction = target - mPosition;
    direction.Normalize();
    Vector2 velocity = direction * speed;
    mRigidBodyComponent->SetVelocity(velocity);
    mMovementDirection = velocity;
    mIsMoving = true;  // Play Run animation
    UpdateFacing(direction);
}

void Enemy::UpdatePatrol(float deltaTime)
{
    // If paused, count down timer before moving to next waypoint
    if (mIsPatrolPaused)
    {
        mPatrolPauseTimer -= deltaTime;
        
        // Stop moving during pause
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
        mMovementDirection = Vector2::Zero;  // No movement direction when paused
        mIsMoving = false;  // Set to false so ManageAnimations plays Idle
        
        if (mPatrolPauseTimer <= 0.0f)
        {
            // Pause complete, switch to next waypoint
            mIsPatrolPaused = false;
            mCurrentWaypoint = (mCurrentWaypoint + 1) % 2;  // Toggle between 0 and 1
            
            if (mGame->IsDebugging())
            {
                SDL_Log("Enemy: Pause complete, now moving toward waypoint %d at (%.2f, %.2f)", 
                        mCurrentWaypoint, 
                        mPatrolWaypoints[mCurrentWaypoint].x, 
                        mPatrolWaypoints[mCurrentWaypoint].y);
            }
            
            // Resume moving
            mIsMoving = true;  // Set to true so ManageAnimations plays Run
        }
        
        return;  // Don't continue with patrol logic while paused
    }
    
    // Get direction to current waypoint
    Vector2 toWaypoint = mPatrolWaypoints[mCurrentWaypoint] - mPosition;
    float distanceToWaypoint = toWaypoint.Length();
    
    if (distanceToWaypoint < WAYPOINT_REACHED_DISTANCE)
    {
        // Reached waypoint, start pause before moving to next one
        mIsPatrolPaused = true;
        mPatrolPauseTimer = mPatrolPauseDuration;
        mIsMoving = false;  // Stop animation immediately
        
        if (mGame->IsDebugging())
        {
            SDL_Log("Enemy: Reached waypoint %d, pausing (%.2f seconds)", 
                    mCurrentWaypoint, mPatrolPauseDuration);
        }
        
        return;  // Start pause immediately
    }
    
    // Check if enemy hit a wall (position didn't change despite velocity)
    float positionChange = (mPosition - mPreviousPosition).Length();
    if (positionChange < MIN_POSITION_CHANGE_TO_DETECT_STUCK && deltaTime > 0.0f)
    {
        // We're stuck, start pause and switch waypoints
        mIsPatrolPaused = true;
        mPatrolPauseTimer = mPatrolPauseDuration;
        mIsMoving = false;  // Stop animation immediately
        
        if (mGame->IsDebugging())
        {
            SDL_Log("Enemy: Hit wall, pausing before switching waypoints (%.2f seconds)", mPatrolPauseDuration);
        }
        
        return;  // Start pause immediately
    }
    
    mPreviousPosition = mPosition;
    
    // Move toward current waypoint
    toWaypoint.Normalize();
    Vector2 velocity = toWaypoint * mPatrolSpeed;
    mRigidBodyComponent->SetVelocity(velocity);
    mMovementDirection = velocity;  // Track movement direction for cone detection
    mIsMoving = true;  // Set to true so ManageAnimations plays Run
    
    UpdateFacing(toWaypoint);
}

void Enemy::UpdateChase(float deltaTime)
{
    const ShadowCat* player = mGame->GetPlayer();
    if (!player) return;
    
    // Update last known position while chasing
    mLastKnownPlayerPos = player->GetPosition();
    
    // Move toward player
    MoveToward(player->GetPosition(), mChaseSpeed);
}

void Enemy::UpdateSearching(float deltaTime)
{
    // Count down search timer
    mSearchTimer += deltaTime;
    
    // Give up searching after duration expires
    if (mSearchTimer >= mSearchDuration)
    {
        mCurrentState = AIState::ReturningToPatrol;
        if (mGame->IsDebugging())
        {
            SDL_Log("Enemy: Search timed out -> ReturningToPatrol");
        }
        return;
    }
    
    // Calculate direction to last known position
    Vector2 toLastKnown = mLastKnownPlayerPos - mPosition;
    float distanceToLastKnown = toLastKnown.Length();
    
    if (distanceToLastKnown < LAST_KNOWN_POSITION_REACHED_DISTANCE)
    {
        mCurrentState = AIState::ReturningToPatrol;
        if (mGame->IsDebugging())
        {
            SDL_Log("Enemy: Reached last known position but player not found -> ReturningToPatrol");
        }
        return;
    }
    
    // Move toward last known position
    MoveToward(mLastKnownPlayerPos, mSearchSpeed);
}

void Enemy::UpdateReturningToPatrol(float deltaTime)
{
    // Find nearest patrol waypoint
    float distToWaypoint0 = (mPatrolWaypoints[0] - mPosition).Length();
    float distToWaypoint1 = (mPatrolWaypoints[1] - mPosition).Length();
    int nearestWaypoint = (distToWaypoint0 < distToWaypoint1) ? 0 : 1;
    
    // Get direction to nearest waypoint
    Vector2 toWaypoint = mPatrolWaypoints[nearestWaypoint] - mPosition;
    float distanceToWaypoint = toWaypoint.Length();
    
    if (distanceToWaypoint < PATROL_AREA_REACHED_DISTANCE)
    {
        mCurrentState = AIState::Patrol;
        mCurrentWaypoint = nearestWaypoint;
        if (mGame->IsDebugging())
        {
            SDL_Log("Enemy: Reached patrol area -> Patrol");
        }
        return;
    }
    
    // Move toward nearest waypoint
    MoveToward(mPatrolWaypoints[nearestWaypoint], mPatrolSpeed);
}

void Enemy::UpdateAttack(float deltaTime)
{
    const ShadowCat* player = mGame->GetPlayer();
    if (!player) return;

    // Stop moving during attack
    mRigidBodyComponent->SetVelocity(Vector2::Zero);
    mMovementDirection = Vector2::Zero;
    mIsMoving = false;  // Play Idle animation

    Vector2 playerPos = player->GetPosition();
    if (mSkill->CanUse(playerPos))
        mSkill->StartSkill(playerPos);
}

void Enemy::OnDebugDraw(class Renderer* renderer)
{
    EnemyDebugDrawer::Draw(renderer, this, mGame);
}

void Enemy::Kill()
{
    if (mIsDead) return;  // Prevent multiple death triggers

    mIsDead = true;
    mIsPlayingDeathAnim = true;
    mDeathTimer = 0.0f;

    // Stop movement
    mRigidBodyComponent->SetVelocity(Vector2(0, 0));
    
    // Lock animations and movement
    SetAnimationLock(true);
    SetMovementLock(true);

    // Disable collider so player can walk through dying enemy
    if (mColliderComponent)
    {
        mColliderComponent->SetEnabled(false);
    }

    // Play death animation
    mAnimatorComponent->PlayAnimation("Death", 2);  // Play twice for effect
}