#include "BasicAttack.h"

#include "../../Actors/Actor.h"
#include "../../Actors/Character.h"
#include "../Drawing/AnimatorComponent.h"
#include "../Drawing/DrawComponent.h"


BasicAttack::BasicAttack(Actor* owner, int updateOrder)
    : SkillBase(owner, updateOrder)
{
    mName = "Basic Attack";
    mDescription = "A simple melee attack.";
    mCooldown = 0.0f;
    mCurrentCooldown = 0.0f;
    mIsAttacking = false;
    mDamageDelay = 0.3f; // Hardcoded for now, want to change later
}

void BasicAttack::Update(float deltaTime)
{
    SkillBase::Update(deltaTime);

    if (!mIsAttacking) return;

    mAttackTimer += deltaTime;
    if (mAttackTimer >= mDamageDelay && !mDamageApplied)
    {
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