#pragma once

#include "../BossBase.h"

class WhiteBoss : public BossBase
{
public:
	WhiteBoss(class Game* game, Vector2 position, float forwardSpeed = 120.0f);

protected:
	void SetupAIBehaviors(const nlohmann::json& data) override;
};