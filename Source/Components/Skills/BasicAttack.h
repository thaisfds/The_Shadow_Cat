#pragma once

#include "SkillBase.h"
#include "../../Math.h"
#include "../Physics/ColliderComponent.h"
#include "../../DelayedActionSystem.h"

class BasicAttack : public SkillBase
{
public:
    BasicAttack(Actor* owner, CollisionFilter filter, int damage, int updateOrder = 100);
    
    void StartSkill(Vector2 targetPosition) override;
    void EndSkill() override;

    void Execute();

private:
    float mDamage;

    CollisionFilter mFilter;
    Collider* mAreaOfEffect;

    nlohmann::json LoadSkillDataFromJSON(const std::string& fileName) override;
};