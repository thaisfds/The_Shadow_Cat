#pragma once

#include "SkillBase.h"
#include "../../Math.h"

class ClawAttack : public SkillBase
{
public:
	ClawAttack(Actor* owner, int updateOrder = 100);
	
	void Update(float deltaTime) override;

	bool CanUse() const override { return !mIsAttacking && SkillBase::CanUse(); }
	
	void Execute() override;
	void EndAttack();
	
private:
	float mConeRadius;
	float mConeAngle;
	int mDamage;

	bool mIsAttacking;
	float mAttackTimer;
	float mAttackDuration;
	float mDamageDelay;
	bool mDamageApplied;
	Vector2 mAttackDirection;
};