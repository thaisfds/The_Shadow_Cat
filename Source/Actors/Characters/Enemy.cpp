#include "Enemy.h"
#include "ShadowCat.h"
#include "../../Game.h"
#include "../../GameConstants.h"
#include "../../Components/Drawing/AnimatorComponent.h"
#include "../../Components/Physics/ColliderComponent.h"
#include "../../Components/Physics/RigidBodyComponent.h"
#include "../../Renderer/Renderer.h"
#include <SDL.h>

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
    , mAttackCooldown(1.5f)
    , mAttackTimer(0.0f)
    , mAttackDamage(1)
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
    
    Collider *collider = new AABBCollider(48, 32);
    mColliderComponent = new ColliderComponent(this, 0, 16, collider, GetBaseEnemyFilter());
    
    // Static enemy - no gravity
    mRigidBodyComponent->SetApplyGravity(false);

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
    mBasicAttack = new BasicAttack(this, mSkillFilter, 1);
}

Enemy::~Enemy()
{
}

void Enemy::OnUpdate(float deltaTime)
{
    Character::OnUpdate(deltaTime);

    // Handle death animation and destruction
    if (mIsPlayingDeathAnim)
    {
        mDeathTimer += deltaTime;
        
        // Destroy actor after death animation completes (0.5 seconds)
        if (mDeathTimer >= 0.5f)
        {
            SetState(ActorState::Destroy);
        }
        return;
    }
    
    // Don't move if dead
    if (mIsDead) return;
    
    // Update attack cooldown timer
    // if (mAttackTimer > 0.0f)
    // {
    //     mAttackTimer -= deltaTime;
    // }
    
    // Check for player detection based on current state
    bool playerInProximity = IsPlayerInProximity();  // Close range (360 degrees)
    bool playerInCone = IsPlayerInRange();  // Cone detection
    bool playerInChaseRange = IsPlayerInChaseRange();  // Circle detection (chase)
    bool playerInAttackRange = IsPlayerInAttackRange();
    
    // State transitions (priority order matters!)
    if (playerInAttackRange && mCurrentState != AIState::Attack)
    {
        mCurrentState = AIState::Attack;
        if (mGame->IsDebugging())
        {
            SDL_Log("Enemy: -> Attack");
        }
    }
    else if (mCurrentState == AIState::Patrol && (playerInProximity || playerInCone))
    {
        // Start chasing if player enters proximity OR cone
        mCurrentState = AIState::Chase;
        if (mGame->IsDebugging())
        {
            SDL_Log("Enemy: -> Chase (%s)", playerInProximity ? "proximity" : "cone detection");
        }
    }
    else if ((mCurrentState == AIState::Chase || mCurrentState == AIState::Attack) && !playerInAttackRange && playerInChaseRange)
    {
        // Stay in chase if already chasing/attacking and player is still in chase range
        if (mCurrentState == AIState::Attack)
        {
            mCurrentState = AIState::Chase;
            if (mGame->IsDebugging())
            {
                SDL_Log("Enemy: Attack -> Chase");
            }
        }
    }
    else if ((mCurrentState == AIState::Chase || mCurrentState == AIState::Attack) && !playerInChaseRange)
    {
        // Player left chase range - enter Searching state
        mCurrentState = AIState::Searching;
        mSearchTimer = 0.0f;  // Reset search timer
        if (mGame->IsDebugging())
        {
            SDL_Log("Enemy: Player lost -> Searching (last known: %.2f, %.2f)", 
                    mLastKnownPlayerPos.x, mLastKnownPlayerPos.y);
        }
    }
    
    // Update detected flag for debug visualization
    mPlayerDetected = (mCurrentState == AIState::Patrol) ? (playerInProximity || playerInCone) : playerInChaseRange;
    
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

bool Enemy::IsPlayerInRange() const
{
    const ShadowCat* player = mGame->GetPlayer();
    if (!player) return false;
    
    Vector2 toPlayer = player->GetPosition() - mPosition;
    float distanceSquared = toPlayer.LengthSq();
    
    // Use chase radius during Searching state, normal detection radius otherwise
    float radius = (mCurrentState == AIState::Searching) ? mChaseDetectionRadius : mDetectionRadius;
    float detectionRadiusSquared = radius * radius;
    
    // Check distance first
    if (distanceSquared > detectionRadiusSquared)
        return false;
    
    // Check if player is within detection cone (in front of enemy)
    Vector2 forward = GetForwardDirection();
    toPlayer.Normalize();
    
    // Calculate angle between forward direction and direction to player
    float dotProduct = Vector2::Dot(forward, toPlayer);
    float angleToPlayer = Math::Acos(dotProduct);
    
    // Player is detected if within half the cone angle
    return angleToPlayer <= (mDetectionAngle / 2.0f);
}

bool Enemy::IsPlayerInProximity() const
{
    const ShadowCat* player = mGame->GetPlayer();
    if (!player) return false;
    
    Vector2 toPlayer = player->GetPosition() - mPosition;
    float distanceSquared = toPlayer.LengthSq();
    float proximityRadiusSquared = mProximityRadius * mProximityRadius;
    
    // Simple circular check - works 360 degrees
    return distanceSquared <= proximityRadiusSquared;
}

bool Enemy::IsPlayerInChaseRange() const
{
    const ShadowCat* player = mGame->GetPlayer();
    if (!player) return false;
    
    Vector2 toPlayer = player->GetPosition() - mPosition;
    float distanceSquared = toPlayer.LengthSq();
    float chaseRadiusSquared = mChaseDetectionRadius * mChaseDetectionRadius;
    
    // Simple circular check - no angle restriction during chase
    return distanceSquared <= chaseRadiusSquared;
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

bool Enemy::IsPlayerInAttackRange() const
{
    const ShadowCat* player = mGame->GetPlayer();
    if (!player) return false;
    
    Vector2 toPlayer = player->GetPosition() - mPosition;
    float distanceSquared = toPlayer.LengthSq();
    float attackRangeSquared = mAttackRange * mAttackRange;
    
    return distanceSquared <= attackRangeSquared;
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
    
    // Check if we've reached the waypoint (within 10 pixels)
    if (distanceToWaypoint < 10.0f)
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
    if (positionChange < 0.1f && deltaTime > 0.0f)
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
    
    // Update sprite facing direction based on movement
    if (toWaypoint.x > 0.0f)
    {
        SetScale(Vector2(1.0f, 1.0f));
    }
    else if (toWaypoint.x < 0.0f)
    {
        SetScale(Vector2(-1.0f, 1.0f));
    }
}

void Enemy::UpdateChase(float deltaTime)
{
    const ShadowCat* player = mGame->GetPlayer();
    if (!player) return;
    
    // Update last known position while chasing
    mLastKnownPlayerPos = player->GetPosition();
    
    // Calculate direction to player
    Vector2 toPlayer = player->GetPosition() - mPosition;
    toPlayer.Normalize();
    
    // Move toward player
    Vector2 velocity = toPlayer * mChaseSpeed;
    mRigidBodyComponent->SetVelocity(velocity);
    mMovementDirection = velocity;  // Track movement direction
    
    // Update sprite facing direction based on movement
    if (toPlayer.x > 0.0f)
    {
        SetScale(Vector2(1.0f, 1.0f));
    }
    else if (toPlayer.x < 0.0f)
    {
        SetScale(Vector2(-1.0f, 1.0f));
    }
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
    
    // Check if player has re-entered detection range
    bool playerInProximity = IsPlayerInProximity();
    bool playerInCone = IsPlayerInRange();
    
    if (playerInProximity || playerInCone)
    {
        // Found the player again!
        // Found the player again!
        mCurrentState = AIState::Chase;
        if (mGame->IsDebugging())
        {
            SDL_Log("Enemy: Player re-detected during search -> Chase");
        }
        return;
    }
    
    // Calculate direction to last known position
    Vector2 toLastKnown = mLastKnownPlayerPos - mPosition;
    float distanceToLastKnown = toLastKnown.Length();
    
    // If we've reached the last known position, give up
    if (distanceToLastKnown < 20.0f)  // Within 20 pixels (close enough)
    {
        mCurrentState = AIState::ReturningToPatrol;
        if (mGame->IsDebugging())
        {
            SDL_Log("Enemy: Reached last known position but player not found -> ReturningToPatrol");
        }
        return;
    }
    
    // Move toward last known position
    toLastKnown.Normalize();
    Vector2 velocity = toLastKnown * mSearchSpeed;
    mRigidBodyComponent->SetVelocity(velocity);
    mMovementDirection = velocity;  // Track movement direction for cone detection
    
    // Update sprite facing direction based on movement
    if (toLastKnown.x > 0.0f)
    {
        SetScale(Vector2(1.0f, 1.0f));
    }
    else if (toLastKnown.x < 0.0f)
    {
        SetScale(Vector2(-1.0f, 1.0f));
    }
}

void Enemy::UpdateReturningToPatrol(float deltaTime)
{
    // Check if player re-enters detection range while returning
    bool playerInProximity = IsPlayerInProximity();
    bool playerInChaseRange = IsPlayerInChaseRange();
    
    if (playerInProximity || playerInChaseRange)
    {
        // Found the player again!
        mCurrentState = AIState::Chase;
        if (mGame->IsDebugging())
        {
            SDL_Log("Enemy: Player detected while returning -> Chase");
        }
        return;
    }
    
    // Find nearest patrol waypoint
    float distToWaypoint0 = (mPatrolWaypoints[0] - mPosition).Length();
    float distToWaypoint1 = (mPatrolWaypoints[1] - mPosition).Length();
    int nearestWaypoint = (distToWaypoint0 < distToWaypoint1) ? 0 : 1;
    
    // Get direction to nearest waypoint
    Vector2 toWaypoint = mPatrolWaypoints[nearestWaypoint] - mPosition;
    float distanceToWaypoint = toWaypoint.Length();
    
    // Check if we've reached the patrol area (within 20 pixels of a waypoint)
    if (distanceToWaypoint < 20.0f)
    {
        // Back in patrol area, resume normal patrol
        // Back in patrol area, resume normal patrol
        mCurrentState = AIState::Patrol;
        mCurrentWaypoint = nearestWaypoint;
        if (mGame->IsDebugging())
        {
            SDL_Log("Enemy: Reached patrol area -> Patrol");
        }
        return;
    }
    
    // Move toward nearest waypoint
    toWaypoint.Normalize();
    Vector2 velocity = toWaypoint * mPatrolSpeed;
    mRigidBodyComponent->SetVelocity(velocity);
    mMovementDirection = velocity;  // Track movement direction for cone detection
    
    // Update sprite facing direction based on movement
    if (toWaypoint.x > 0.0f)
    {
        SetScale(Vector2(1.0f, 1.0f));
    }
    else if (toWaypoint.x < 0.0f)
    {
        SetScale(Vector2(-1.0f, 1.0f));
    }
}

void Enemy::UpdateAttack(float deltaTime)
{
    const ShadowCat* player = mGame->GetPlayer();
    if (!player) return;

    if (mBasicAttack->CanUse())
        mBasicAttack->Execute(player->GetPosition());
    
    // // Stop moving during attack
    // mRigidBodyComponent->SetVelocity(Vector2::Zero);
    // mMovementDirection = Vector2::Zero;  // No movement during attack
    
    // // Face the player
    // Vector2 toPlayer = player->GetPosition() - mPosition;
    // if (toPlayer.x > 0.0f)
    // {
    //     SetScale(Vector2(1.0f, 1.0f));
    // }
    // else if (toPlayer.x < 0.0f)
    // {
    //     SetScale(Vector2(-1.0f, 1.0f));
    // }
    
    // // Attack if cooldown is ready
    // if (mAttackTimer <= 0.0f)
    // {
    //     // Deal damage to player (need to cast away const)
    //     const_cast<ShadowCat*>(player)->TakeDamage(mAttackDamage);
        
    //     // Reset cooldown
    //     mAttackTimer = mAttackCooldown;
        
    //     if (mGame->IsDebugging())
    //     {
    //         SDL_Log("Enemy attacked player for %d damage!", mAttackDamage);
    //     }
        
    //     // Play hit animation (brief attack indication)
    //     mAnimatorComponent->PlayAnimationOnce("Hit");
    // }
}

void Enemy::OnDebugDraw(class Renderer* renderer)
{
    if (!mGame->IsDebugging()) return;
    
    Vector3 detectionColor = mPlayerDetected ? Vector3(1.0f, 0.0f, 0.0f) : Vector3(1.0f, 1.0f, 0.0f); // Red : Yellow
    
    // Draw different visualization based on state
    if (mCurrentState == AIState::Patrol)
    {
        // Draw detection cone during patrol
        Vector2 forward = GetForwardDirection();
        float baseAngle = Math::Atan2(forward.y, forward.x);
        
        const int segments = 16;
        float halfConeAngle = mDetectionAngle / 2.0f;
        float startAngle = baseAngle - halfConeAngle;
        float angleStep = mDetectionAngle / segments;
        
        // Draw the arc
        for (int i = 0; i < segments; i++)
        {
            float angle1 = startAngle + i * angleStep;
            float angle2 = startAngle + (i + 1) * angleStep;
            
            Vector2 p1 = mPosition + Vector2(
                Math::Cos(angle1) * mDetectionRadius,
                Math::Sin(angle1) * mDetectionRadius
            );
            
            Vector2 p2 = mPosition + Vector2(
                Math::Cos(angle2) * mDetectionRadius,
                Math::Sin(angle2) * mDetectionRadius
            );
            
            renderer->DrawRect(p1, Vector2(4.0f, 4.0f), 0.0f, detectionColor, mGame->GetCameraPos(), RendererMode::LINES);
        }
        
        // Draw cone edges
        Vector2 leftEdge = mPosition + Vector2(
            Math::Cos(startAngle) * mDetectionRadius,
            Math::Sin(startAngle) * mDetectionRadius
        );
        Vector2 rightEdge = mPosition + Vector2(
            Math::Cos(startAngle + mDetectionAngle) * mDetectionRadius,
            Math::Sin(startAngle + mDetectionAngle) * mDetectionRadius
        );
        
        const int edgePoints = 8;
        for (int i = 0; i <= edgePoints; i++)
        {
            float t = i / (float)edgePoints;
            Vector2 leftPoint = mPosition + (leftEdge - mPosition) * t;
            Vector2 rightPoint = mPosition + (rightEdge - mPosition) * t;
            
            renderer->DrawRect(leftPoint, Vector2(4.0f, 4.0f), 0.0f, detectionColor, mGame->GetCameraPos(), RendererMode::LINES);
            renderer->DrawRect(rightPoint, Vector2(4.0f, 4.0f), 0.0f, detectionColor, mGame->GetCameraPos(), RendererMode::LINES);
        }
        
        // Always draw proximity circle (white/light blue color)
        Vector3 proximityColor = Vector3(0.8f, 0.8f, 1.0f); // Light blue
        const int proxSegments = 24;
        const float proxAngleStep = Math::TwoPi / proxSegments;
        
        for (int i = 0; i < proxSegments; i++)
        {
            float angle1 = i * proxAngleStep;
            
            Vector2 p1 = mPosition + Vector2(
                Math::Cos(angle1) * mProximityRadius,
                Math::Sin(angle1) * mProximityRadius
            );
            
            renderer->DrawRect(p1, Vector2(3.0f, 3.0f), 0.0f, proximityColor, mGame->GetCameraPos(), RendererMode::LINES);
        }
    }
    else if (mCurrentState == AIState::Searching)
    {
        // Draw search area visualization - orange/yellow color to indicate searching
        Vector3 searchColor = Vector3(1.0f, 0.6f, 0.0f); // Orange
        const int segments = 32;
        const float angleStep = Math::TwoPi / segments;
        
        // Draw detection cone (still active during search) - uses chase radius for extended range
        Vector2 forward = GetForwardDirection();
        float baseAngle = Math::Atan2(forward.y, forward.x);
        
        const int coneSegments = 16;
        float halfConeAngle = mDetectionAngle / 2.0f;
        float startAngle = baseAngle - halfConeAngle;
        float coneAngleStep = mDetectionAngle / coneSegments;
        
        for (int i = 0; i < coneSegments; i++)
        {
            float angle1 = startAngle + i * coneAngleStep;
            
            Vector2 p1 = mPosition + Vector2(
                Math::Cos(angle1) * mChaseDetectionRadius,
                Math::Sin(angle1) * mChaseDetectionRadius
            );
            
            renderer->DrawRect(p1, Vector2(3.0f, 3.0f), 0.0f, searchColor, mGame->GetCameraPos(), RendererMode::LINES);
        }
        
        // Draw proximity circle
        Vector3 proximityColor = Vector3(0.8f, 0.8f, 1.0f);
        const int proxSegments = 24;
        const float proxAngleStep = Math::TwoPi / proxSegments;
        
        for (int i = 0; i < proxSegments; i++)
        {
            float angle1 = i * proxAngleStep;
            
            Vector2 p1 = mPosition + Vector2(
                Math::Cos(angle1) * mProximityRadius,
                Math::Sin(angle1) * mProximityRadius
            );
            
            renderer->DrawRect(p1, Vector2(3.0f, 3.0f), 0.0f, proximityColor, mGame->GetCameraPos(), RendererMode::LINES);
        }
        
        // Draw last known position marker (X shape)
        Vector3 markerColor = Vector3(1.0f, 0.0f, 1.0f); // Magenta
        float markerSize = 16.0f;
        
        // Draw X at last known position
        for (int i = -1; i <= 1; i++)
        {
            renderer->DrawRect(
                mLastKnownPlayerPos + Vector2(i * 4.0f, i * 4.0f),
                Vector2(5.0f, 5.0f),
                0.0f,
                markerColor,
                mGame->GetCameraPos(),
                RendererMode::LINES
            );
            renderer->DrawRect(
                mLastKnownPlayerPos + Vector2(i * 4.0f, -i * 4.0f),
                Vector2(5.0f, 5.0f),
                0.0f,
                markerColor,
                mGame->GetCameraPos(),
                RendererMode::LINES
            );
        }
        
        // Draw line from enemy to last known position
        Vector2 toLastKnown = mLastKnownPlayerPos - mPosition;
        float distance = toLastKnown.Length();
        const int linePoints = 10;
        
        for (int i = 0; i <= linePoints; i++)
        {
            float t = i / (float)linePoints;
            Vector2 point = mPosition + toLastKnown * t;
            renderer->DrawRect(point, Vector2(3.0f, 3.0f), 0.0f, markerColor, mGame->GetCameraPos(), RendererMode::LINES);
        }
    }
    else
    {
        // Draw chase circle during chase/attack states
        const int segments = 32;
        const float angleStep = Math::TwoPi / segments;
        
        for (int i = 0; i < segments; i++)
        {
            float angle1 = i * angleStep;
            float angle2 = (i + 1) * angleStep;
            
            Vector2 p1 = mPosition + Vector2(
                Math::Cos(angle1) * mChaseDetectionRadius,
                Math::Sin(angle1) * mChaseDetectionRadius
            );
            
            Vector2 p2 = mPosition + Vector2(
                Math::Cos(angle2) * mChaseDetectionRadius,
                Math::Sin(angle2) * mChaseDetectionRadius
            );
            
            renderer->DrawRect(p1, Vector2(4.0f, 4.0f), 0.0f, detectionColor, mGame->GetCameraPos(), RendererMode::LINES);
        }
        
        // Always draw proximity circle (white/light blue color)
        Vector3 proximityColor = Vector3(0.8f, 0.8f, 1.0f); // Light blue
        const int proxSegments = 24;
        const float proxAngleStep = Math::TwoPi / proxSegments;
        
        for (int i = 0; i < proxSegments; i++)
        {
            float angle1 = i * proxAngleStep;
            
            Vector2 p1 = mPosition + Vector2(
                Math::Cos(angle1) * mProximityRadius,
                Math::Sin(angle1) * mProximityRadius
            );
            
            renderer->DrawRect(p1, Vector2(3.0f, 3.0f), 0.0f, proximityColor, mGame->GetCameraPos(), RendererMode::LINES);
        }
    }
    
    // Always draw patrol waypoints (green squares) for all states
    Vector3 waypointColor = Vector3(0.0f, 1.0f, 0.0f); // Green
    Vector3 currentWaypointColor = Vector3(0.0f, 1.0f, 1.0f); // Cyan for current target
    
    for (int i = 0; i < 2; i++)
    {
        Vector3 color = (i == mCurrentWaypoint && mCurrentState == AIState::Patrol) ? currentWaypointColor : waypointColor;
        
        // Draw a square at each waypoint
        for (int dx = -8; dx <= 8; dx += 4)
        {
            for (int dy = -8; dy <= 8; dy += 4)
            {
                renderer->DrawRect(
                    mPatrolWaypoints[i] + Vector2(dx, dy),
                    Vector2(4.0f, 4.0f),
                    0.0f,
                    color,
                    mGame->GetCameraPos(),
                    RendererMode::LINES
                );
            }
        }
    }
    
    // Draw line between waypoints
    Vector2 waypointDiff = mPatrolWaypoints[1] - mPatrolWaypoints[0];
    const int linePoints = 15;
    for (int i = 0; i <= linePoints; i++)
    {
        float t = i / (float)linePoints;
        Vector2 point = mPatrolWaypoints[0] + waypointDiff * t;
        renderer->DrawRect(point, Vector2(2.0f, 2.0f), 0.0f, waypointColor, mGame->GetCameraPos(), RendererMode::LINES);
    }
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
