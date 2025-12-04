#pragma once
#include <functional>
#include <map>

#include "../Component.h"
#include <string>
#include <vector>
#include "../../Math.h"
#include "../../DelayedActionSystem.h"

class Character;

class SkillBase : public Component
{
public:
    SkillBase(class Actor* owner, int updateOrder = 100);
    virtual ~SkillBase() = default;

    void Update(float deltaTime) override;
    
    virtual bool CanUse() const;
    
    virtual void StartSkill(Vector2 targetPosition);
    virtual void EndSkill() { mIsUsing = false; }
    
    float GetCooldown() const { return mCurrentCooldown; }
    bool IsOnCooldown() const { return mCurrentCooldown > 0.0f; }
    
    const std::string& GetName() const { return mName; }
    const std::string& GetDescription() const { return mDescription; }

protected:
    class Character *mCharacter;

    std::string mName;
    std::string mDescription;
    float mCooldown;
    float mCurrentCooldown;
    bool mIsUsing;
    Vector2 mTargetVector; // Can be either direction or position depending on skill

    struct DelayedAction
    {
        float delay;
        std::function<void()> action;
        bool executed = false;
    };

    class DelayedActionSystem mDelayedActions;

    void AddDelayedAction(float delay, std::function<void()> action)
    {
        mDelayedActions.AddDelayedAction(delay, action);
    }
};