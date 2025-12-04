#pragma once

#include "SkillBase.h"
#include "../../Math.h"

class ClawAttack : public SkillBase
{
public:
	ClawAttack(Actor* owner, int updateOrder = 100);
	
	void StartSkill(Vector2 targetPosition) override;
	void EndSkill() override;

	void Execute();
	
private:
	float mConeRadius;
	float mConeAngle;
	int mDamage;
};