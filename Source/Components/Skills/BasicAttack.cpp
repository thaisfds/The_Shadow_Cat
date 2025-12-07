#include "BasicAttack.h"

#include "../AnimatedParticleSystemComponent.h"
#include "../../Actors/Actor.h"
#include "../../Actors/Characters/Character.h"
#include "../Drawing/AnimatorComponent.h"
#include "../Drawing/DrawComponent.h"
#include "../Physics/Physics.h"
#include "../../Game.h"


BasicAttack::BasicAttack(Actor* owner, CollisionFilter filter, int damage, int updateOrder,
                         float coneRadius, float coneAngleDegrees)
    : SkillBase(owner, updateOrder)
{
    mName = "Basic Attack";
    mDescription = "A simple melee attack.";
    mCooldown = 2.0f;
    mCurrentCooldown = 0.0f;
    mIsAttacking = false;
    mDamageDelay = 0.53f; // Hardcoded for now, want to change later
    mFilter = filter;
    mDamage = damage;

    mAttackDuration = mCharacter->GetComponent<AnimatorComponent>()->GetAnimationDuration("BasicAttack");
    if (mAttackDuration == 0.0f) mAttackDuration = 1.0f;

    mConeRadius = coneRadius;
    mConeAngle = Math::ToRadians(coneAngleDegrees);
}

void BasicAttack::Update(float deltaTime)
{
    SkillBase::Update(deltaTime);

    if (!mIsAttacking) return;

    mAttackTimer += deltaTime;
    if (mAttackTimer >= mDamageDelay && !mDamageApplied)
    {
        mDamageApplied = true;
        // This should emit particle only when damaging, not here. Also, make a playOnce for animatedParticles
        mCharacter->GetGame()->GetAttackTrailActor()->GetComponent<AnimatedParticleSystemComponent>()->EmitParticleAt(
            0.3f,
            0.0f,
            mCharacter->GetPosition() + mAttackDirection * mConeRadius,
            std::atan2(mAttackDirection.y, mAttackDirection.x),
            mCharacter->GetScale().x < 0.0f
        );

        auto hitColliders = Physics::ConeCast(mCharacter->GetGame(), mCharacter->GetPosition(), mAttackDirection, mConeAngle, mConeRadius, mFilter);
        for (auto collider : hitColliders)
        {
            auto enemyActor = collider->GetOwner();
            auto enemyCharacter = dynamic_cast<Character*>(enemyActor);
            enemyCharacter->TakeDamage(mDamage);
        }
    }

    if (mAttackTimer >= mAttackDuration) EndAttack();
}

void BasicAttack::Execute(Vector2 targetPosition)
{
    mCharacter->GetComponent<AnimatorComponent>()->PlayAnimationOnce("BasicAttack");
    mCharacter->SetMovementLock(true);
    mCharacter->SetAnimationLock(true);  // Lock animations so attack anim isn't overridden
    
    mIsAttacking = true;
    mAttackTimer = 0.0f;
    mDamageApplied = false;
    
    mAttackDirection = targetPosition - mCharacter->GetPosition();
    mAttackDirection.Normalize();

    if (mAttackDirection.x > 0.0f)
        mCharacter->SetScale(Vector2(1.0f, mCharacter->GetScale().y));
    else if (mAttackDirection.x < 0.0f)
        mCharacter->SetScale(Vector2(-1.0f, mCharacter->GetScale().y));
    
    StartCooldown();
}

void BasicAttack::EndAttack()
{
    mIsAttacking = false;
    mCharacter->SetMovementLock(false);
    mCharacter->SetAnimationLock(false);  // Unlock animations
}