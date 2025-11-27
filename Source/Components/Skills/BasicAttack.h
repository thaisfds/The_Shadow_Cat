#pragma once

#include "SkillBase.h"
#include "../../Math.h"

class BasicAttack : public SkillBase
{
public:
    BasicAttack(Actor* owner, int updateOrder = 100);
    
    void Update(float deltaTime) override;

    bool CanUse() const override { return !mIsAttacking && SkillBase::CanUse(); }
    
    void Execute() override;
    void EndAttack();

private:
    float mConeRadius;
    float mConeAngle;

    bool mIsAttacking;
    float mAttackTimer;
    float mDamageDelay;
    bool mDamageApplied;
    Vector2 mAttackDirection;
};