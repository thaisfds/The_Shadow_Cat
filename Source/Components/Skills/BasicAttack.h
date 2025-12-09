#pragma once

#include "SkillBase.h"
#include "../../Math.h"
#include "../Physics/ColliderComponent.h"

class BasicAttack : public SkillBase
{
public:
    BasicAttack(Actor* owner, CollisionFilter filter, int damage, int updateOrder = 100,
                float coneRadius = 50.0f, float coneAngleDegrees = 45.0f);
    
    void Update(float deltaTime) override;

    bool CanUse() const override { return !mIsAttacking && SkillBase::CanUse(); }
    
    void Execute(Vector2 targetPosition) override;
    void EndAttack();

private:
    float mConeRadius;
    float mConeAngle;
    int mDamage;

    bool mIsAttacking;
    float mAttackTimer;
    float mAttackDuration;
    float mDamageDelay;
    bool mDamageApplied;
    Vector2 mAttackDirection;
    CollisionFilter mFilter;
};