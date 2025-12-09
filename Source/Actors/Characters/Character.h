#pragma once
#include "../Actor.h"
#include "../../Components/Physics/CollisionFilter.h"

class RigidBodyComponent;
class AnimatorComponent;
class ColliderComponent;

class Character : public Actor
{
public:
    Character(class Game* game, float forwardSpeed = 200.0f);
    virtual ~Character();

    void OnUpdate(float deltaTime) override;

    virtual void TakeDamage(int damage);
    void Kill() override;

    bool GetAnimationLock() const { return mIsAnimationLocked; }
    void SetAnimationLock(bool isLocked);

    bool GetMovementLock() const { return mIsMovementLocked; }
    void SetMovementLock(bool isLocked);
    
    bool IsDead() const { return mIsDead; }

    void SetSpeedMultiplier(float multiplier);

    virtual void ResetCollisionFilter() const = 0;

    CollisionFilter GetSkillFilter() const { return mSkillFilter; }

    static CollisionFilter GetBaseEnemyFilter()
    {
        CollisionFilter filter;
        filter.belongsTo = CollisionFilter::GroupMask({CollisionGroup::Enemy});
        filter.collidesWith = CollisionFilter::GroupMask({CollisionGroup::Player, CollisionGroup::Enemy, CollisionGroup::Environment, CollisionGroup::PlayerSkills});
        return filter;
    }

protected:
    void ManageAnimations();

    int hp;

    class RigidBodyComponent *mRigidBodyComponent;
    class AnimatorComponent *mAnimatorComponent;
    class ColliderComponent *mColliderComponent;
    class CollisionFilter mSkillFilter;

    float mForwardSpeed;
    float mSpeedMultiplier = 1.0f;

    bool mIsMoving;

    bool mIsAnimationLocked;
    bool mIsMovementLocked;

    bool mIsDead;
};