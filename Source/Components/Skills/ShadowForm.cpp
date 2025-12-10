#include "ShadowForm.h"
#include "../../Actors/Characters/Character.h"
#include "../Drawing/AnimatorComponent.h"
#include "../../SkillFactory.h"

ShadowForm::ShadowForm(Actor* owner, int updateOrder)
	: SkillBase(owner, updateOrder)
{
	LoadSkillDataFromJSON("ShadowFormData");

	
}

nlohmann::json ShadowForm::LoadSkillDataFromJSON(const std::string& fileName)
{
	auto data = SkillBase::LoadSkillDataFromJSON(fileName);

	mDuration = GameJsonParser::GetFloatValue(data, "duration");
	mSpeed = GameJsonParser::GetFloatEffectValue(data, "speedMultiplier");
	auto id = GameJsonParser::GetStringValue(data, "id");
	SkillFactory::Instance().RegisterSkill(id, [](Actor* owner) { return new ShadowForm(owner); });

	mUpgrades.push_back(GameJsonParser::GetUpgradeInfo(this, data, "cooldown", &mCooldown));
	mUpgrades.push_back(GameJsonParser::GetUpgradeInfo(this, data, "speedMultiplier", &mSpeed));
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

	float shadowFormBeginDuration = animator->GetAnimationDuration("ShadowFormBegin");
	AddDelayedAction(shadowFormBeginDuration, [this]() { mCharacter->SetMovementLock(false); });
	float shadowFormEndDelay = mDuration - animator->GetAnimationDuration("ShadowFormEnd");
	AddDelayedAction(shadowFormEndDelay, [this]()
	{ 
		mCharacter->SetMovementLock(true);
		mCharacter->GetComponent<AnimatorComponent>()->PlayAnimationOnce("ShadowFormEnd", false); 
	});
	AddDelayedAction(mDuration, [this]() { EndSkill(); });
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