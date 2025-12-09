#pragma once

#include "SkillBase.h"
class Bomb : public SkillBase
{
public:
	Bomb(Actor* owner, int updateOrder = 100);
	virtual ~Bomb() = default;

	nlohmann::json LoadSkillDataFromJSON(const std::string& fileName) override;

	void StartSkill(Vector2 targetPosition) override;
};