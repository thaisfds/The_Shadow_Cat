#pragma once

#include "../AIBehavior.h"

class SkillBehavior : public AIBehavior
{
public:
	SkillBehavior(Character* owner);

	void OnEnter() override;
	void Update(float deltaTime) override;
	void OnExit() override;

	void LoadBehaviorData(const nlohmann::json& data) override {}

	bool SkillToPatrol();
	bool AnySkillAvailable() const;
};