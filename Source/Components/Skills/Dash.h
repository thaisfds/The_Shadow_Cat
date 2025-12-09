#pragma once

#include "SkillBase.h"
#include "../../Math.h"

class Dash : public SkillBase
{
public:
	Dash(Actor* owner, int updateOrder = 100);
	
	void Update(float deltaTime) override;

	void StartSkill(Vector2 targetPosition) override;
	void EndSkill() override;
	
private:
	float mDashSpeed;

	nlohmann::json LoadSkillDataFromJSON(const std::string& fileName) override;
};