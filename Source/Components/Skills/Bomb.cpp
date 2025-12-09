#include "Bomb.h"

Bomb::Bomb(Actor* owner, int updateOrder)
	: SkillBase(owner, updateOrder)
{
	LoadSkillDataFromJSON("BombData");
}

nlohmann::json Bomb::LoadSkillDataFromJSON(const std::string& fileName)
{
	auto data = SkillBase::LoadSkillDataFromJSON(fileName);

	return data;
}

void Bomb::StartSkill(Vector2 targetPosition)
{
	mCurrentCooldown = mCooldown;

	
}