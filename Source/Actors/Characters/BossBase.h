#pragma once

#include "EnemyBase.h"

class BossBase : public EnemyBase
{
public:
	BossBase(class Game* game, Vector2 position, float forwardSpeed);

	void Kill() override;
};