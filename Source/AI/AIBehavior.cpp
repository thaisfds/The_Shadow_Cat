#include "AIBehavior.h"

std::string AIBehavior::CheckTransitions() const
{
	for (const auto& transition : mTransitions)
	{
		if (!transition.mCondition()) continue;
		
		return transition.mTargetState;
	}
	return "";
}

bool AIBehavior::CheckTransitionConditions() const
{
	for (const auto& condition : mTransitionConditions)
		if (condition()) return true;
	return false;
}