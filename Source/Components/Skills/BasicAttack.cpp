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

    mAttackDuration = mCharacter->GetComponent<AnimatorComponent>()->GetAnimationDuration("BasicAttack");
    if (mAttackDuration == 0.0f) mAttackDuration = 1.0f;

    mConeRadius = 100.0f;
    mConeAngle = Math::ToRadians(30.0f);
    mDamage = 10;
}

void BasicAttack::Update(float deltaTime)
{
    SkillBase::Update(deltaTime);

    if (!mIsAttacking) return;

    mAttackTimer += deltaTime;
    if (mAttackTimer >= mDamageDelay && !mDamageApplied)
    {
        mDamageApplied = true;
        auto hitColliders = PhysicsUtils::ConeCast(mCharacter->GetGame(), mCharacter->GetPosition(), mAttackDirection, mConeAngle, mConeRadius, ColliderLayer::Enemy);
        for (auto collider : hitColliders)
        {
            auto enemyActor = collider->GetOwner();
            auto enemyCharacter = dynamic_cast<Character*>(enemyActor);
            enemyCharacter->TakeDamage(mDamage);
        }
    }

    if (mAttackTimer >= mAttackDuration) EndAttack();
}

void BasicAttack::Execute()
{
    mCharacter->GetComponent<AnimatorComponent>()->PlayAnimationOnce("BasicAttack");
    
    mIsAttacking = true;
    mAttackTimer = 0.0f;
    mDamageApplied = false;
    
    Vector2 mouseWorldPos = mCharacter->GetGame()->GetMouseWorldPosition();
    mAttackDirection = mouseWorldPos - mCharacter->GetPosition();
    mAttackDirection.Normalize();
    
    mCharacter->SetMovementLock(true);
}

void BasicAttack::EndAttack()
{
    mIsAttacking = false;
    mCharacter->SetMovementLock(false);
    StartCooldown();
}