#pragma once

#include "../EnemyBase.h"
#include "../../../AI/Behaviors/FleeBehavior.h"
#include "../../../AI/Behaviors/PatrolBehavior.h"
#include "../../../AI/Behaviors/SkillBehavior.h"

class SylvesterCat : public EnemyBase
{
public:
	explicit SylvesterCat(Game* game, Vector2 position);

	void OnUpdate(float deltaTime) override;

	void SetFleeTimer(float time) { mFleeTimer = time; }

protected:
	void SetupAIBehaviors(const nlohmann::json& data) override;

private:
	float mFleeTimer;

	PatrolBehavior *mPatrolBehavior;
	FleeBehavior *mFleeBehavior;
	SkillBehavior *mSkillBehavior;
};