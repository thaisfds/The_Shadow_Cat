#include "BasicAttack.h"

#include "../../Actors/Actor.h"
#include "../../Actors/Characters/Character.h"
#include "../Drawing/AnimatorComponent.h"
#include "../Drawing/DrawComponent.h"
#include "../Physics/AABBColliderComponent.h"
#include "../Physics/PhysicsUtils.h"
#include "../../Game.h"


BasicAttack::BasicAttack(Actor* owner, int updateOrder)
    : SkillBase(owner, updateOrder)
{
    mName = "Basic Attack";
    mDescription = "A simple melee attack.";
    mCooldown = 0.0f;
    mCurrentCooldown = 0.0f;
    mIsAttacking = false;
    mDamageDelay = 0.3f; // Hardcoded for now, want to change later

    mConeRadius = 100.0f;
    mConeAngle = Math::ToRadians(30.0f);
}

void BasicAttack::Update(float deltaTime)
{
    SkillBase::Update(deltaTime);

    if (!mIsAttacking) return;

    mAttackTimer += deltaTime;
    if (mAttackTimer >= mDamageDelay && !mDamageApplied)
    {
        if (PhysicsUtils::ConeCast(mCharacter->GetGame(), mCharacter->GetPosition(), mAttackDirection, mConeAngle, mConeRadius, ColliderLayer::Enemy))
            SDL_Log("Basic Attack hit an enemy!");

        // Apply damage to target here
        mDamageApplied = true;
    }

    if (mAttackTimer >= 0.6f) EndAttack();
}

void BasicAttack::Execute()
{
    mCharacter->GetComponent<AnimatorComponent>()->SetAnimation("BasicAttack");
    
    mIsAttacking = true;
    mAttackTimer = 0.0f;
    mDamageApplied = false;
    
    Vector2 mouseWorldPos = mCharacter->GetGame()->GetMouseWorldPosition();
    mAttackDirection = mouseWorldPos - mCharacter->GetPosition();
    mAttackDirection.Normalize();
    
    mCharacter->SetAnimationLock(true);
    mCharacter->SetMovementLock(true);
}

void BasicAttack::EndAttack()
{
    mIsAttacking = false;
    mCharacter->SetAnimationLock(false);
    mCharacter->SetMovementLock(false);
    StartCooldown();
}