#pragma once
#include <functional>
#include <map>

#include "../Component.h"
#include <string>
#include <vector>
#include "../../Math.h"
#include "../../DelayedActionSystem.h"
#include "../../Json.h"
#include "../../GameJsonParser.h"
#include "UpgradeInfo.h"

class Character;

class SkillBase : public Component
{
public:
    SkillBase(class Actor* owner, int updateOrder = 100);
    virtual ~SkillBase() = default;

    void Update(float deltaTime) override;
    void ComponentDraw(class Renderer* renderer) override;
    
    virtual bool CanUse(Vector2 targetPosition, bool showRangeOnFalse = false) const;
    virtual bool EnemyShouldUse() { return false; }
    
    virtual void StartSkill(Vector2 targetPosition);
    virtual void EndSkill();
    
    float GetCooldown() const { return mCurrentCooldown; }
    bool IsOnCooldown() const { return mCurrentCooldown > 0.0f; }
    
    const std::string& GetName() const { return mName; }
    const std::string& GetDescription() const { return mDescription; }
    std::vector<UpgradeInfo> GetAvailableUpgrades() const;

protected:
    class Character *mCharacter;

    std::string mName;
    std::string mDescription;
    float mCooldown;
    float mCurrentCooldown;
    float mRange;
    bool mIsUsing;
    Vector2 mTargetVector; // Can be either direction or position depending on skill

    mutable float mDrawRangeTimer = 0.0f;

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

    std::vector<UpgradeInfo> mUpgrades;

    void RegisterUpgrade(const std::string& type, float value, int maxLevel, float* variable);
    void ApplyUpgrade(const std::string& upgradeType);
    bool CanUpgrade(const std::string& upgradeType) const;
    
    virtual nlohmann::json LoadSkillDataFromJSON(const std::string& fileName);
};