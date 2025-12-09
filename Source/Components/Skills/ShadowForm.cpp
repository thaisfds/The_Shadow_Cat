#include "ShadowForm.h"
#include "../../Actors/Characters/Character.h"
#include "../Drawing/AnimatorComponent.h"

ShadowForm::ShadowForm(Actor* owner, int updateOrder)
	: SkillBase(owner, updateOrder)
{
	LoadSkillDataFromJSON("ShadowFormData");

	float shadowFormBeginDuration = mCharacter->GetComponent<AnimatorComponent>()->GetAnimationDuration("ShadowFormBegin");
	AddDelayedAction(shadowFormBeginDuration, [this]() { mCharacter->SetMovementLock(false); });
	float shadowFormEndDelay = mDuration - mCharacter->GetComponent<AnimatorComponent>()->GetAnimationDuration("ShadowFormEnd");
	AddDelayedAction(shadowFormEndDelay, [this]()
	{ 
		mCharacter->SetMovementLock(true);
		mCharacter->GetComponent<AnimatorComponent>()->PlayAnimationOnce("ShadowFormEnd", false); 
	});
	AddDelayedAction(mDuration, [this]() { EndSkill(); });
}

nlohmann::json ShadowForm::LoadSkillDataFromJSON(const std::string& fileName)
{
	auto data = SkillBase::LoadSkillDataFromJSON(fileName);

	mDuration = SkillJsonParser::GetFloatValue(data, "duration");
	mSpeed = SkillJsonParser::GetFloatEffectValue(data, "speedMultiplier");

	mUpgrades.push_back(SkillJsonParser::GetUpgradeInfo(data, "duration", &mDuration));
	mUpgrades.push_back(SkillJsonParser::GetUpgradeInfo(data, "cooldown", &mCooldown));
	mUpgrades.push_back(SkillJsonParser::GetUpgradeInfo(data, "speedMultiplier", &mSpeed));
	return data;
}

void ShadowForm::StartSkill(Vector2 targetPosition)
{
	SkillBase::StartSkill(targetPosition);
	mCharacter->SetAnimationLock(true);
	auto animator = mCharacter->GetComponent<AnimatorComponent>();
	animator->LoopAnimation("ShadowFormMid");
	animator->PlayAnimationOnce("ShadowFormBegin");
	animator->SetAnimOffset(Vector2(0.0f, 14.0f));

	mCharacter->SetSpeedMultiplier(mSpeed);
	
	mCharacter->SetMovementLock(true);

	CollisionFilter filter = mCharacter->GetComponent<ColliderComponent>()->GetFilter();
	filter.collidesWith = CollisionFilter::RemoveGroups(filter.collidesWith,
		{CollisionGroup::Player, CollisionGroup::Enemy, CollisionGroup::PlayerSkills, CollisionGroup::EnemySkills});
	mCharacter->GetComponent<ColliderComponent>()->SetFilter(filter);
}

void ShadowForm::EndSkill()
{
	SkillBase::EndSkill();

	auto animator = mCharacter->GetComponent<AnimatorComponent>();
	animator->SetAnimOffset(Vector2::Zero);

	mCharacter->SetSpeedMultiplier(1.0f);

	mCharacter->SetAnimationLock(false);
	mCharacter->SetMovementLock(false);

	mCharacter->ResetCollisionFilter();
}