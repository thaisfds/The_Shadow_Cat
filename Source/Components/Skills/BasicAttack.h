#pragma once

#include "SkillBase.h"

class BasicAttack : public SkillBase
{
public:
    BasicAttack(Actor* owner, int updateOrder = 100);
	
	void Update(float deltaTime) override;
	
    void Execute() override;
	void EndAttack();

private:
	bool mIsAttacking;
	float mAttackTimer;
	float mDamageDelay;
	bool mDamageApplied;
};