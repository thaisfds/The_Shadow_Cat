#pragma once

#include "SkillBase.h"

class ShadowForm : public SkillBase
{
public:
	ShadowForm(Actor* owner, int updateOrder = 100);
	virtual ~ShadowForm() = default;

	nlohmann::json LoadSkillDataFromJSON(const std::string& fileName) override;

	void StartSkill(Vector2 targetPosition) override;
	void EndSkill() override;

private:
	float mDuration;
	float mSpeed;
};