#pragma once

#include "SkillBase.h"
#include "../../Math.h"
#include "../Physics/ColliderComponent.h"
#include "../../DelayedActionSystem.h"

class BasicAttack : public SkillBase
{
public:
    BasicAttack(Actor* owner, int updateOrder = 100);
    
    void StartSkill(Vector2 targetPosition) override;
    void EndSkill() override;

    bool EnemyShouldUse() override;

    void Execute();

private:
    float mDamage;
    Collider* mAreaOfEffect;

    nlohmann::json LoadSkillDataFromJSON(const std::string& fileName) override;
};