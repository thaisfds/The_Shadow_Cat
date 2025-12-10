#pragma once
#include <vector>
#include <functional>
#include <string>
#include "../Actors/Characters/Character.h"
#include "../Json.h"

struct StateTransition
{
    std::string mTargetState;
    std::function<bool()> mCondition;
};

class AIBehavior
{
public:
    AIBehavior(Character *owner, const char* name) : mOwner(owner), mName(name) {}
    virtual ~AIBehavior() = default;
    
    virtual void OnEnter() {}
    virtual void Update(float deltaTime) = 0;
    virtual void OnExit() {}
    
    const char* GetName() const { return mName; }
    
    void AddTransition(const std::string& targetState, std::function<bool()> condition)
    {
        mTransitions.push_back({targetState, condition});
    }

    void AddTransitionCondition(std::function<bool()> condition)
    {
        mTransitionConditions.push_back(condition);
    }
    
    const std::vector<StateTransition>& GetTransitions() const { return mTransitions; }
    
	std::string CheckTransitions() const;

    bool CheckTransitionConditions() const;

    virtual void LoadBehaviorData(const nlohmann::json& data) = 0;

protected:
    const char* mName;
    std::vector<StateTransition> mTransitions;
    std::vector<std::function<bool()>> mTransitionConditions;
    Character *mOwner;
};