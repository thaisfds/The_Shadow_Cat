#include "BossBase.h"
#include "../../Game.h"

BossBase::BossBase(class Game* game, Vector2 position, float forwardSpeed)
	: EnemyBase(game, position, forwardSpeed)
{
	mGame->RegisterBoss(this);
}

void BossBase::Kill()
{
	mGame->UnregisterBoss(this);
	EnemyBase::Kill();
}