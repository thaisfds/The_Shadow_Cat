#include "BasicAttack.h"

#include "../AnimatedParticleSystemComponent.h"
#include "../../Actors/Actor.h"
#include "../../Actors/Characters/Character.h"
#include "../Drawing/AnimatorComponent.h"
#include "../Drawing/DrawComponent.h"
#include "../Physics/Physics.h"
#include "../../Game.h"
#include "SkillBase.h"


BasicAttack::BasicAttack(Actor* owner, CollisionFilter filter, int damage, int updateOrder)
    : SkillBase(owner, updateOrder)
{
    LoadSkillDataFromJSON("BasicAttackData");

    mFilter = filter;
    
    float attackDuration = mCharacter->GetComponent<AnimatorComponent>()->GetAnimationDuration("BasicAttack");
    if (attackDuration == 0.0f) attackDuration = 1.0f;
    AddDelayedAction(0.53f, [this]() { Execute(); });
    AddDelayedAction(attackDuration, [this]() { EndSkill(); });
}

nlohmann::json BasicAttack::LoadSkillDataFromJSON(const std::string& fileName)
{
    auto data = SkillBase::LoadSkillDataFromJSON(fileName);

    mDamage = SkillJsonParser::GetFloatEffectValue(data, "damage");
    mConeAngle = Math::ToRadians(SkillJsonParser::GetFloatValue(data, "angle"));

    return data;
}

void BasicAttack::Execute()
{
    mCharacter->GetGame()->GetAttackTrailActor()->GetComponent<AnimatedParticleSystemComponent>()->EmitParticleAt(
        0.3f,
        0.0f,
        mCharacter->GetPosition() + mTargetVector * mRange,
        std::atan2(mTargetVector.y, mTargetVector.x),
        mCharacter->GetScale().x < 0.0f
    );

    auto hitColliders = Physics::ConeCast(mCharacter->GetGame(), mCharacter->GetPosition(), mTargetVector, mConeAngle, mRange, mFilter);
    for (auto collider : hitColliders)
    {
        auto enemyActor = collider->GetOwner();
        auto enemyCharacter = dynamic_cast<Character*>(enemyActor);
        enemyCharacter->TakeDamage(mDamage);
    }
}

void BasicAttack::StartSkill(Vector2 targetPosition)
{
    SkillBase::StartSkill(targetPosition);

    mCharacter->GetComponent<AnimatorComponent>()->PlayAnimationOnce("BasicAttack");
    mCharacter->SetMovementLock(true);
    mCharacter->SetAnimationLock(true);  // Lock animations so attack anim isn't overridden
}

void BasicAttack::EndSkill()
{
    SkillBase::EndSkill();
    
    mCharacter->SetMovementLock(false);
    mCharacter->SetAnimationLock(false);  // Unlock animations
}