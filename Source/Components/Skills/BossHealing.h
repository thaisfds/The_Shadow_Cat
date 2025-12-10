#pragma once

#include "SkillBase.h"
#include "../../Math.h"
#include "../../DelayedActionSystem.h"

class BossHealing : public SkillBase
{
public:
    BossHealing(Actor* owner, int updateOrder = 100);
    
    void StartSkill(Vector2 targetPosition) override;
    void EndSkill() override;

    void Execute();

    // Check if healing should trigger at current health percentage
    bool ShouldTriggerHealing();

private:
    float mHealPercent; // Percentage of max HP to heal (0.2 = 20%)
    bool mHealedAt60;
    bool mHealedAt40;
    bool mHealedAt20;

    nlohmann::json LoadSkillDataFromJSON(const std::string& fileName) override;
};

