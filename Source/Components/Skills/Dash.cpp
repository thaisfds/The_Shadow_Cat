#include "Dash.h"
#include "../../Actors/Characters/Character.h"
#include "../../Components/Physics/RigidBodyComponent.h"
#include "../../Game.h"
#include "../Drawing/AnimatorComponent.h"
#include "../Physics/ColliderComponent.h"

Dash::Dash(Actor* owner, int updateOrder)
	: SkillBase(owner, updateOrder)
{
	LoadSkillDataFromJSON("DashData");

	float dashDuration = mRange / mDashSpeed;
	float dashEndDelay = dashDuration - mCharacter->GetComponent<AnimatorComponent>()->GetAnimationDuration("DashEnd");
	AddDelayedAction(dashEndDelay, [this]() { 
		mCharacter->GetComponent<AnimatorComponent>()->PlayAnimationOnce("DashEnd", false); 
	});
	AddDelayedAction(dashDuration, [this]() { EndSkill(); });
}

nlohmann::json Dash::LoadSkillDataFromJSON(const std::string& fileName)
{
	auto data = SkillBase::LoadSkillDataFromJSON(fileName);

	mDashSpeed = GameJsonParser::GetFloatEffectValue(data, "speed");

	mUpgrades.push_back(GameJsonParser::GetUpgradeInfo(data, "range", &mRange));
	mUpgrades.push_back(GameJsonParser::GetUpgradeInfo(data, "speed", &mDashSpeed));
	mUpgrades.push_back(GameJsonParser::GetUpgradeInfo(data, "cooldown", &mCooldown));

	return data;
}

void Dash::Update(float deltaTime)
{
	SkillBase::Update(deltaTime);

	if (!mIsUsing) return;
	
	Vector2 dashVelocity = mTargetVector * mDashSpeed;
	mCharacter->GetComponent<RigidBodyComponent>()->SetVelocity(dashVelocity);
}

void Dash::StartSkill(Vector2 targetPosition)
{
	SkillBase::StartSkill(targetPosition);
	mTargetVector -= mCharacter->GetPosition();
	mTargetVector.Normalize();

	mCharacter->SetAnimationLock(true);
	AnimatorComponent* animator = mCharacter->GetComponent<AnimatorComponent>();
	animator->LoopAnimation("DashMid");
	animator->PlayAnimationOnce("DashBegin");	

	mCharacter->SetMovementLock(true);

	CollisionFilter filter = mCharacter->GetComponent<ColliderComponent>()->GetFilter();
	filter.collidesWith = CollisionFilter::RemoveGroups(filter.collidesWith,
		{CollisionGroup::Player, CollisionGroup::Enemy, CollisionGroup::PlayerSkills, CollisionGroup::EnemySkills});
	mCharacter->GetComponent<ColliderComponent>()->SetFilter(filter);
}

void Dash::EndSkill()
{
	SkillBase::EndSkill();

	mCharacter->SetAnimationLock(false);
	mCharacter->SetMovementLock(false);

	mCharacter->ResetCollisionFilter();
}