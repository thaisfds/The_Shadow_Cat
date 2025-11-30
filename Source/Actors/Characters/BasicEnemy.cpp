#include "BasicEnemy.h"
#include "ShadowCat.h"
#include "../../Game.h"
#include "../../GameConstants.h"
#include "../../Components/Drawing/AnimatorComponent.h"
#include "../../Components/Physics/AABBColliderComponent.h"
#include "../../Components/Physics/RigidBodyComponent.h"
#include "../../Renderer/Renderer.h"
#include <SDL.h>

BasicEnemy::BasicEnemy(class Game* game, float forwardSpeed, float patrolDistance)
    : Character(game, forwardSpeed)
    , mDeathTimer(0.0f)
    , mIsPlayingDeathAnim(false)
    , mPatrolDistance(patrolDistance)
    , mPatrolDirection(1)
    , mPatrolSpeed(50.0f)
    , mDetectionRadius(200.0f)
    , mPlayerDetected(false)
{
    // Use WhiteCat sprite
    mAnimatorComponent = new AnimatorComponent(this, "ShadowCatAnim", GameConstants::TILE_SIZE, GameConstants::TILE_SIZE);
    mRigidBodyComponent = new RigidBodyComponent(this);
    CollisionFilter collisionFilter = {
        CollisionFilter::GroupMask({CollisionGroup::Enemy}),
        CollisionFilter::GroupMask({CollisionGroup::Environment, CollisionGroup::Enemy, CollisionGroup::PlayerSkills})
    };
    mColliderComponent = new AABBColliderComponent(this, 0, 0, GameConstants::TILE_SIZE, GameConstants::TILE_SIZE, collisionFilter);
    
    // Static enemy - no gravity
    mRigidBodyComponent->SetApplyGravity(false);

    // Set enemy health (lower than player's 10)
    hp = 30;

    // Setup animations
    mAnimatorComponent->AddAnimation("Idle", {1});  // Frame 1 is Idle
    mAnimatorComponent->AddAnimation("Run", {0, 2});  // Frames 0 and 2 for running
    mAnimatorComponent->AddAnimation("Hit", {0, 1});  // Flicker between frames for hit effect
    mAnimatorComponent->AddAnimation("Death", {2, 1, 0});  // Run animation frames as death effect

    mAnimatorComponent->LoopAnimation("Run");
    
    // Patrol start position will be set when position is first updated
    mPatrolStartPos = Vector2::Zero;
}

BasicEnemy::~BasicEnemy()
{
}

void BasicEnemy::OnUpdate(float deltaTime)
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
    
    // Check for player detection
    mPlayerDetected = IsPlayerInRange();
    
    if (mGame->IsDebugging() && mPlayerDetected)
    {
        SDL_Log("BasicEnemy detected player!");
    }
    
    // Set patrol start position on first update (when position is valid)
    if (mPatrolStartPos.x == 0.0f && mPatrolStartPos.y == 0.0f)
    {
        mPatrolStartPos = mPosition;
        if (mGame->IsDebugging())
        {
            SDL_Log("BasicEnemy patrol starting at: (%.2f, %.2f)", mPatrolStartPos.x, mPatrolStartPos.y);
        }
    }
    
    // Patrol movement
    float distanceFromStart = mPosition.x - mPatrolStartPos.x;
    
    // Check if we need to turn around
    if (mPatrolDirection == 1 && distanceFromStart >= mPatrolDistance)
    {
        mPatrolDirection = -1;
    }
    else if (mPatrolDirection == -1 && distanceFromStart <= -mPatrolDistance)
    {
        mPatrolDirection = 1;
    }
    
    // Set velocity for patrol movement
    Vector2 velocity(mPatrolDirection * mPatrolSpeed, 0.0f);
    mRigidBodyComponent->SetVelocity(velocity);
    
    // Update sprite facing direction
    if (mPatrolDirection > 0)
    {
        SetScale(Vector2(1.0f, 1.0f));
    }
    else
    {
        SetScale(Vector2(-1.0f, 1.0f));
    }
}

void BasicEnemy::TakeDamage(int damage)
{
    if (mIsDead) return;  // Don't take damage if already dead

    if (mGame->IsDebugging())
    {
        SDL_Log("BasicEnemy taking %d damage. HP before: %d", damage, hp);
    }
    
    hp = Math::Max(hp - damage, 0);
    
    if (mGame->IsDebugging())
    {
        SDL_Log("HP after: %d", hp);
    }
    
    hp = Math::Max(hp - damage, 0);
    
    if (mGame->IsDebugging())
    {
        SDL_Log("HP after: %d", hp);
    }
    
    if (hp <= 0)
    {
        Kill();
    }
    else
    {
        // Play hit animation
        mAnimatorComponent->PlayAnimationOnce("Hit");
    }
}

bool BasicEnemy::IsPlayerInRange() const
{
    const ShadowCat* player = mGame->GetPlayer();
    if (!player) return false;
    
    Vector2 toPlayer = player->GetPosition() - mPosition;
    float distanceSquared = toPlayer.LengthSq();
    float detectionRadiusSquared = mDetectionRadius * mDetectionRadius;
    
    return distanceSquared <= detectionRadiusSquared;
}

void BasicEnemy::OnDebugDraw(Renderer* renderer)
{
    if (!mGame->IsDebugging()) return;
    
    // Draw detection radius as a circle (approximated with points)
    Vector3 detectionColor = mPlayerDetected ? Vector3(1.0f, 0.0f, 0.0f) : Vector3(1.0f, 1.0f, 0.0f); // Red : Yellow
    
    // Draw detection circle by drawing multiple points
    const int segments = 32;
    const float angleStep = Math::TwoPi / segments;
    
    for (int i = 0; i < segments; i++)
    {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;
        
        Vector2 p1 = mPosition + Vector2(
            Math::Cos(angle1) * mDetectionRadius,
            Math::Sin(angle1) * mDetectionRadius
        );
        
        Vector2 p2 = mPosition + Vector2(
            Math::Cos(angle2) * mDetectionRadius,
            Math::Sin(angle2) * mDetectionRadius
        );
        
        // Draw small rects at each point to form a circle
        renderer->DrawRect(p1, Vector2(4.0f, 4.0f), 0.0f, detectionColor, mGame->GetCameraPos(), RendererMode::LINES);
    }
}

void BasicEnemy::Kill()
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
