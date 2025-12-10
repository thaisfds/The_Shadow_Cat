#include "Bomb.h"
#include "../../SkillFactory.h"

Bomb::Bomb(Actor* owner, int updateOrder)
	: SkillBase(owner, updateOrder)
{
	LoadSkillDataFromJSON("BombData");
}

nlohmann::json Bomb::LoadSkillDataFromJSON(const std::string& fileName)
{
	auto data = SkillBase::LoadSkillDataFromJSON(fileName);

	auto id = GameJsonParser::GetStringValue(data, "id");
	SkillFactory::Instance().RegisterSkill(id, [](Actor* owner) { return new Bomb(owner); });

	return data;
}

void Bomb::StartSkill(Vector2 targetPosition)
{
	mCurrentCooldown = mCooldown;

	
}