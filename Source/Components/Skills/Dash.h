#pragma once

#include "SkillBase.h"
#include "../../Math.h"

class Dash : public SkillBase
{
public:
	Dash(Actor* owner, int updateOrder = 100);
	
	void Update(float deltaTime) override;

	bool CanUse() const override { return !mIsDashing && SkillBase::CanUse(); }
	
	void Execute(Vector2 targetPosition) override;
	void EndDash();
	
private:
	bool mIsDashing;
	float mDashTimer;
	Vector2 mDashDirection;
	
	float mDashDuration;
	float mDashSpeed;
};