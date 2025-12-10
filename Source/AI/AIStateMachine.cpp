#include "AIStateMachine.h"

AIStateMachine::AIStateMachine(class Actor* owner)
	: mOwner(owner),
	  mCurrentState(nullptr)
{
}

AIStateMachine::~AIStateMachine()
{
	for (auto& pair : mStateNameToBehavior) delete pair.second;
	mStateNameToBehavior.clear();
}

void AIStateMachine::RegisterState(AIBehavior* behavior)
{
	auto name = behavior->GetName();
	mStateNameToBehavior[name] = behavior;
}

void AIStateMachine::SetInitialState(const std::string& stateName)
{
	auto it = mStateNameToBehavior.find(stateName);
	if (it == mStateNameToBehavior.end()) return;

	mCurrentState = it->second;
	if (mCurrentState) mCurrentState->OnEnter();
}

const char* AIStateMachine::GetCurrentStateName() const
{
	return mCurrentState ? mCurrentState->GetName() : "None";
}

void AIStateMachine::Update(float deltaTime)
{
    CheckTransitions();
    
    if (mCurrentState) mCurrentState->Update(deltaTime);
}

void AIStateMachine::CheckTransitions()
{
    if (!mCurrentState) return;

	for (auto& state : mStateNameToBehavior)
	{
		if (state.second->CheckTransitionConditions())
		{
			TransitionTo(state.second);
			return;
		}
	}
    
    std::string targetStateName = mCurrentState->CheckTransitions();
    if (targetStateName.empty()) return;

	auto stateIt = mStateNameToBehavior.find(targetStateName);
	if (stateIt != mStateNameToBehavior.end())
	{
		TransitionTo(stateIt->second);
		return;
	}
}

void AIStateMachine::TransitionTo(AIBehavior* newState)
{
    if (mCurrentState == newState) return;
    
    if (mCurrentState) mCurrentState->OnExit();
    
    mCurrentState = newState;
    if (mCurrentState) mCurrentState->OnEnter();
}