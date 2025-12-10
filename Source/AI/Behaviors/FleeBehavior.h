#pragma once

#include "../AIBehavior.h"

class FleeBehavior : public AIBehavior
{
public:
	FleeBehavior(Character* owner, SkillBase* fleeSkill, float fleeDistance = 200.0f);
	
	void OnEnter() override;
	void Update(float deltaTime) override;
	void OnExit() override;

	void LoadBehaviorData(const nlohmann::json& data) override;

	float GetFleeDistance() const { return mFleeDistance; }
	bool ShouldLeaveState() const { return mLeaveState; }

private:
	float mFleeDistance;
	float mSpeedMultiplier;
	float mFleeCooldown;
	Vector2 mLastPosition;
	SkillBase *mFleeSkill;

	bool mLeaveState;
};