#pragma once

#include "SkillBase.h"
#include "../../Math.h"

class ClawAttack : public SkillBase
{
public:
	ClawAttack(Actor* owner, int updateOrder = 100);
	
	void Update(float deltaTime) override;

	void StartSkill(Vector2 targetPosition) override;
	void EndSkill() override;

	void Execute();
	
private:
	float mConeAngle;
	float mDamage;
	float mForwardSpeed;
	float mBackwardDistancePercentage;
	float mBackwardSpeed;

	Vector2 mVelocity;

	nlohmann::json LoadSkillDataFromJSON(const std::string& fileName) override;
};