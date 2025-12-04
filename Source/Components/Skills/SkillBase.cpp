#include "SkillBase.h"
#include "../../Actors/Characters/Character.h"

SkillBase::SkillBase(Actor* owner, int updateOrder)
	: Component(owner, updateOrder)
	, mName("Unnamed Skill")
	, mDescription("No description available.")
	, mCooldown(0.0f)
	, mCurrentCooldown(0.0f)
	, mCharacter(dynamic_cast<Character*>(owner))
{
}

void SkillBase::Update(float deltaTime)
{
	if (mCurrentCooldown > 0.0f)
	{
		mCurrentCooldown -= deltaTime;
		if (mCurrentCooldown < 0.0f) mCurrentCooldown = 0.0f;
	}

	if (!mIsUsing) return;

	mDelayedActions.Update(deltaTime);
}

void SkillBase::StartSkill(Vector2 targetPosition)
{
	mCurrentCooldown = mCooldown;
	mIsUsing = true;
	mDelayedActions.Reset();

	mTargetVector = targetPosition - mCharacter->GetPosition();
	mTargetVector.Normalize();

	if (mTargetVector.x < 0.0f)
		mCharacter->SetScale(Vector2(-1.0f, 1.0f));
	else
		mCharacter->SetScale(Vector2(1.0f, 1.0f));
}


bool SkillBase::CanUse() const
{
	return mCurrentCooldown <= 0.0f;
}

