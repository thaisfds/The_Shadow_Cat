#include "../Actor.h"
#include "Character.h"
#include "../../Components/Physics/RigidBodyComponent.h"
#include "../../Components/Drawing/AnimatorComponent.h"
#include "../../Game.h"
#include "../../GameConstants.h"

Character::Character(class Game *game, float forwardSpeed)
    : Actor(game)
    , mIsAnimationLocked(false)
    , mIsMovementLocked(false)
    , mForwardSpeed(forwardSpeed)
    , mIsDead(false)
    , mIsMoving(false)
    , hp(10)
{
}

Character::~Character()
{
}

void Character::OnUpdate(float deltaTime)
{
    Vector2 pos = GetPosition();
    const float margin = 15.0f;
    const float maxX = GameConstants::LEVEL_WIDTH * GameConstants::TILE_SIZE - margin;
    const float maxY = GameConstants::LEVEL_HEIGHT * GameConstants::TILE_SIZE - margin;

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
    SetAnimationLock(true);
    SetMovementLock(true);
}

void Character::ManageAnimations()
{
    if (mIsDead) return;
    if (mIsAnimationLocked) return;

    if (mIsMoving) mAnimatorComponent->LoopAnimation("Run");
    else mAnimatorComponent->LoopAnimation("Idle");
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