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
	mIsAttacking = false;
	mDamageDelay = 1.3f; // Hardcoded for now, want to change later

	mAttackDuration = mCharacter->GetComponent<AnimatorComponent>()->GetAnimationDuration("ClawAttack");
	if (mAttackDuration == 0.0f) mAttackDuration = 1.0f;

	mConeRadius = 60.0f;
	mConeAngle = Math::ToRadians(60.0f);
	mDamage = 20;
}

void ClawAttack::Update(float deltaTime)
{
	SkillBase::Update(deltaTime);

	if (!mIsAttacking) return;

	mAttackTimer += deltaTime;
	if (mAttackTimer >= mDamageDelay && !mDamageApplied)
	{
		mDamageApplied = true;

		CollisionFilter filter;
		filter.belongsTo = CollisionFilter::GroupMask({CollisionGroup::PlayerSkills});
		filter.collidesWith = CollisionFilter::GroupMask({CollisionGroup::Enemy});

		auto hitColliders = Physics::ConeCast(mCharacter->GetGame(), mCharacter->GetPosition(), mAttackDirection, mConeAngle, mConeRadius, filter);
		for (auto collider : hitColliders)
		{
			auto enemyActor = collider->GetOwner();
			auto enemyCharacter = dynamic_cast<Character*>(enemyActor);
			enemyCharacter->TakeDamage(mDamage);
		}
	}

	if (mAttackTimer >= mAttackDuration) EndAttack();
}

void ClawAttack::Execute(Vector2 targetPosition)
{
	mCharacter->GetComponent<AnimatorComponent>()->PlayAnimationOnce("ClawAttack");
	mCharacter->SetMovementLock(true);

	mIsAttacking = true;
	mAttackTimer = 0.0f;
	mDamageApplied = false;

	Vector2 mouseWorldPos = mCharacter->GetGame()->GetMouseWorldPosition();
	mAttackDirection = mouseWorldPos - mCharacter->GetPosition();
	mAttackDirection.Normalize();

	if (mAttackDirection.x < 0.0f)
		mCharacter->SetScale(Vector2(-1.0f, 1.0f));
	else
		mCharacter->SetScale(Vector2(1.0f, 1.0f));

	StartCooldown();
}

void ClawAttack::EndAttack()
{
	mIsAttacking = false;
	mCharacter->SetMovementLock(false);
}