#pragma once

#include <vector>
#include <functional>

class DelayedActionSystem
{
public:
    struct DelayedAction
    {
        float delay;
        std::function<void()> action;
        bool executed = false;
    };
    
    void AddDelayedAction(float delay, std::function<void()> action)
    {
        mDelayedActions.push_back({delay, action, false});
    }
    
    void Update(float deltaTime);
    void Reset();
    
    void Clear() { mDelayedActions.clear(); }

private:
    std::vector<DelayedAction> mDelayedActions;
    float mTimer = 0.0f;
};