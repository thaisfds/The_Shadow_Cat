#include "SkillBase.h"
#include "../../Actors/Characters/Character.h"
#include <fstream>

#include "../../Game.h"
#include "../../GameConstants.h"

SkillBase::SkillBase(Actor* owner, int updateOrder)
	: Component(owner, updateOrder)
	, mCurrentCooldown(0.0f)
	, mCharacter(dynamic_cast<Character*>(owner))
{
}

nlohmann::json SkillBase::LoadSkillDataFromJSON(const std::string& fileName)
{
	std::ifstream skillFile("../Assets/Data/Skill/" + fileName + ".json");

	if (!skillFile.is_open())
	{
		SDL_Log("Failed to open skill file: %s", (fileName).c_str());
		return nullptr;
	}

	nlohmann::json skillData = nlohmann::json::parse(skillFile);
	if (skillData.is_null())
	{
		SDL_Log("Failed to parse skill file: %s", (fileName).c_str());
		return nullptr;
	}

	mName = SkillJsonParser::GetStringValue(skillData, "name");
	mDescription = SkillJsonParser::GetStringValue(skillData, "description");
	mCooldown = SkillJsonParser::GetFloatValue(skillData, "cooldown");
	mRange = SkillJsonParser::GetFloatValue(skillData, "range");

	return skillData;
}


void SkillBase::Update(float deltaTime)
{
	if (mCurrentCooldown > 0.0f) mCurrentCooldown -= deltaTime;
	if (mDrawRangeTimer > 0.0f) mDrawRangeTimer -= deltaTime;

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

void SkillBase::ComponentDraw(class Renderer* renderer)
{
	if (mDrawRangeTimer <= 0.0f) return;
	renderer->DrawCircle(mCharacter->GetPosition(), mRange, Color::Red, mCharacter->GetGame()->GetCameraPos());
}

bool SkillBase::CanUse(Vector2 targetPosition, bool showRangeOnFalse) const
{
	return mCurrentCooldown <= 0.0f;
}

