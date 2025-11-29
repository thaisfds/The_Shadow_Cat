#include "BasicEnemy.h"
#include "../../Game.h"
#include "../../GameConstants.h"
#include "../../Components/Drawing/AnimatorComponent.h"
#include "../../Components/Physics/RigidBodyComponent.h"
#include "../../Components/Physics/AABBColliderComponent.h"

BasicEnemy::BasicEnemy(class Game* game, float forwardSpeed)
    : Character(game, forwardSpeed)
    , mDeathTimer(0.0f)
    , mIsPlayingDeathAnim(false)
{
    // Use WhiteCat sprite
    mAnimatorComponent = new AnimatorComponent(this, "../Assets/Sprites/WhiteCat/WhiteCat.png", 
                                               "../Assets/Sprites/WhiteCat/WhiteCat.json", 
                                               GameConstants::TILE_SIZE, GameConstants::TILE_SIZE);
    mRigidBodyComponent = new RigidBodyComponent(this);
    mColliderComponent = new AABBColliderComponent(this, 0, 0, GameConstants::TILE_SIZE, GameConstants::TILE_SIZE, ColliderLayer::Enemy);
    
    // Static enemy - no gravity
    mRigidBodyComponent->SetApplyGravity(false);

    // Set enemy health (lower than player's 10)
    hp = 30;

    // Setup animations
    mAnimatorComponent->AddAnimation("Idle", {1});  // Frame 1 is Idle
    mAnimatorComponent->AddAnimation("Hit", {0, 1});  // Flicker between frames for hit effect
    mAnimatorComponent->AddAnimation("Death", {2, 1, 0});  // Run animation frames as death effect

    mAnimatorComponent->LoopAnimation("Idle");
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
