#include "DelayedActionSystem.h"

void DelayedActionSystem::Update(float deltaTime)
{
	mTimer += deltaTime;

	for (auto& action : mDelayedActions)
	{
		if (action.executed) continue;

		if (mTimer >= action.delay)
		{
			action.action();
			action.executed = true;
		}
	}
}

void DelayedActionSystem::Reset()
{
	mTimer = 0.0f;
	for (auto& action : mDelayedActions) action.executed = false;
}