#pragma once

#include "../BossBase.h"

class OrangeBoss : public BossBase
{
public:
	OrangeBoss(class Game *game, Vector2 position, float forwardSpeed = 120.0f);

	void OnUpdate(float deltaTime) override;

protected:
	void SetupAIBehaviors(const nlohmann::json &data) override;

private:
	void CheckAndTriggerHealing();
	// Footstep sound system
	float mFootstepTimer;
};
