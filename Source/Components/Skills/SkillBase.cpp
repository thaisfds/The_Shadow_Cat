#include "SkillBase.h"
#include "../../Actors/Character.h"

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
	if (mCurrentCooldown <= 0.0f) return;
	
	mCurrentCooldown -= deltaTime;
	if (mCurrentCooldown < 0.0f) mCurrentCooldown = 0.0f;
}

bool SkillBase::CanUse() const
{
	return mCurrentCooldown <= 0.0f;
}

void SkillBase::ProcessInput(const uint8_t* keyState) {}