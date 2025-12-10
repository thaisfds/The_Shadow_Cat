#include "../Actor.h"
#include "Character.h"
#include "../../Components/Physics/RigidBodyComponent.h"
#include "../../Components/Drawing/AnimatorComponent.h"
#include "../../Game.h"
#include "../../GameConstants.h"

Character::Character(class Game *game, Vector2 position, float forwardSpeed)
    : Actor(game)
    , mIsAnimationLocked(false)
    , mIsMovementLocked(false)
    , mForwardSpeed(forwardSpeed)
    , mIsDead(false)
    , mIsMoving(false)
    , hp(10)
    , mAnimatorComponent(nullptr)
    , mIsUsingSkill(false)
{
    mPosition = position;
    mRigidBodyComponent = new RigidBodyComponent(this);
    
    Collider *collider = new AABBCollider(48, 32);
    mColliderComponent = new ColliderComponent(this, Vector2(0, 16), collider);
}

Character::Character(class Game *game, float forwardSpeed)
    : Actor(game)
    , mIsAnimationLocked(false)
    , mIsMovementLocked(false)
    , mForwardSpeed(forwardSpeed)
    , mIsDead(false)
    , mIsMoving(false)
    , hp(10)
    , mAnimatorComponent(nullptr)
{
    mRigidBodyComponent = new RigidBodyComponent(this);
    
    Collider *collider = new AABBCollider(48, 32);
    mColliderComponent = new ColliderComponent(this, Vector2(0, 16), collider);
}

Character::~Character()
{
}

void Character::OnUpdate(float deltaTime)
{
    Vector2 pos = GetPosition();
    const float margin = 15.0f;
    
    // Use actual level dimensions from Game instead of constants
    const float maxX = mGame->GetLevelWidth() * GameConstants::TILE_SIZE - margin;
    const float maxY = mGame->GetLevelHeight() * GameConstants::TILE_SIZE - margin;

    mPosition.x = Math::Clamp(mPosition.x, margin, maxX);
    mPosition.y = Math::Clamp(mPosition.y, margin, maxY);

    ManageAnimations();
}

void Character::TakeDamage(int damage)
{
    hp -= damage;
    if (hp <= 0) Kill();
}

void Character::Kill()
{
    mIsDead = true;
    mRigidBodyComponent->SetVelocity(Vector2(0, 0));
    mAnimatorComponent->LoopAnimation("Idle");
    SetAnimationLock(true);
    SetMovementLock(true);
    mState = ActorState::Destroy;
}

void Character::ManageAnimations()
{
    if (mIsDead) return;
    if (mIsAnimationLocked) return;
    if (!mAnimatorComponent) return;

    if (mIsMoving) mAnimatorComponent->LoopAnimation("Run");
    else mAnimatorComponent->LoopAnimation("Idle");
}

void Character::MoveToward(const Vector2& target)
{
	if (mIsMovementLocked) return;
	
	Vector2 direction = target - mPosition;
	direction.Normalize();
	Vector2 velocity = direction * mForwardSpeed;
	mRigidBodyComponent->SetVelocity(velocity);
	mIsMoving = true;
	UpdateFacing(direction);
}

void Character::UpdateFacing(const Vector2& direction)
{
    if (direction.x > 0.0f)
        SetScale(Vector2(1.0f, 1.0f));
    else if (direction.x < 0.0f)
        SetScale(Vector2(-1.0f, 1.0f));
}

void Character::StopMovement()
{
    mRigidBodyComponent->SetVelocity(Vector2::Zero);
    mIsMoving = false;
}

Vector2 Character::GetForward() const
{
    auto rigidBody = GetComponent<RigidBodyComponent>();
    if (rigidBody)
    {
        Vector2 velocity = rigidBody->GetVelocity();
        if (!Math::NearlyZero(velocity.LengthSq()))
        {
            Vector2 forward = velocity;
            forward.Normalize();
            return forward;
        }
    }

    return (mScale.x >= 0.0f) ? Vector2(1.0f, 0.0f) : Vector2(-1.0f, 0.0f);
}

void Character::SetAnimationLock(bool isLocked)
{
    mIsAnimationLocked = isLocked;
}

void Character::SetMovementLock(bool isLocked)
{
    mRigidBodyComponent->SetVelocity(Vector2(0, 0));
    mIsMovementLocked = isLocked;
}

void Character::SetSpeedMultiplier(float multiplier)
{
    mForwardSpeed = mForwardSpeed / mSpeedMultiplier * multiplier;
    mSpeedMultiplier = multiplier;
}