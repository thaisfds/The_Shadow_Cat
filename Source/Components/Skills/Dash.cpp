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

	mDashSpeed = SkillJsonParser::GetFloatEffectValue(data, "speed");

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

	mCharacter->GetComponent<ColliderComponent>()->SetFilter(
		Character::GetBasePlayerFilter()
	);
}