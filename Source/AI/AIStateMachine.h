#pragma once

#include "AIBehavior.h"
#include <unordered_map>
#include <vector>
#include <functional>
#include <string>

class AIStateMachine
{
public:
    AIStateMachine(class Actor* owner);
    ~AIStateMachine();
    
    void Update(float deltaTime);
    void TransitionTo(AIBehavior* newState);
    
    AIBehavior* GetCurrentState() const { return mCurrentState; }
    const char* GetCurrentStateName() const;
    
    void RegisterState(AIBehavior* behavior);
    void SetInitialState(const std::string& stateName);
    
    Actor* GetOwner() const { return mOwner; }
    
private:
    Actor* mOwner;
    AIBehavior* mCurrentState;
    
    std::unordered_map<std::string, AIBehavior*> mStateNameToBehavior;
    
    void CheckTransitions();
};