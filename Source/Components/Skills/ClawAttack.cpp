#include "ClawAttack.h"
#include "../../Game.h"
#include "../../Actors/Characters/Character.h"
#include "../Drawing/AnimatorComponent.h"
#include "../Physics/Physics.h"
#include "../Physics/ColliderComponent.h"

ClawAttack::ClawAttack(Actor* owner, int updateOrder)
	: SkillBase(owner, updateOrder)
{
	mName = "Claw Attack";
	mDescription = "Perform a ferocious pounce, slashing with your claws.";
	mCooldown = 3.0f;
	mCurrentCooldown = 0.0f;

	
	mConeRadius = 60.0f;
	mConeAngle = Math::ToRadians(60.0f);
	mDamage = 20;
	
	float attackDuration = mCharacter->GetComponent<AnimatorComponent>()->GetAnimationDuration("ClawAttack");
	if (attackDuration == 0.0f) attackDuration = 1.0f;
	AddDelayedAction(1.3f, [this]() { Execute(); });
	AddDelayedAction(attackDuration, [this]() { EndSkill(); });
}

void ClawAttack::Execute()
{
	CollisionFilter filter;
	filter.belongsTo = CollisionFilter::GroupMask({CollisionGroup::PlayerSkills});
	filter.collidesWith = CollisionFilter::GroupMask({CollisionGroup::Enemy});

	auto hitColliders = Physics::ConeCast(mCharacter->GetGame(), mCharacter->GetPosition(), mTargetVector, mConeAngle, mConeRadius, filter);
	for (auto collider : hitColliders)
	{
		auto enemyActor = collider->GetOwner();
		auto enemyCharacter = dynamic_cast<Character*>(enemyActor);
		enemyCharacter->TakeDamage(mDamage);
	}
}

void ClawAttack::StartSkill(Vector2 targetPosition)
{
	SkillBase::StartSkill(targetPosition);

	mCharacter->GetComponent<AnimatorComponent>()->PlayAnimationOnce("ClawAttack");
	mCharacter->SetMovementLock(true);
}

void ClawAttack::EndSkill()
{
	SkillBase::EndSkill();

	mCharacter->SetMovementLock(false);
}