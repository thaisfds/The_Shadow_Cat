#include "BossBase.h"
#include "../../Game.h"
#include "../../LevelManager.h"

BossBase::BossBase(class Game* game, Vector2 position, float forwardSpeed)
	: EnemyBase(game, position, forwardSpeed)
{
	LevelManager::Instance().RegisterBoss(this);
}

void BossBase::Kill()
{
	LevelManager::Instance().UnregisterBoss(this);
	EnemyBase::Kill();
}