#pragma once
#include "../Actor.h"
#include "../../Components/Physics/CollisionFilter.h"
#include "../../Components/Skills/SkillBase.h"
#include <vector>

class RigidBodyComponent;
class AnimatorComponent;
class ColliderComponent;

class Character : public Actor
{
public:
    Character(class Game* game, Vector2 position, float forwardSpeed = 200.0f);
    Character(class Game* game, float forwardSpeed = 200.0f);
    virtual ~Character();

    void OnUpdate(float deltaTime) override;

    virtual void TakeDamage(int damage);
    void Kill() override;

    void MoveToward(const Vector2& target);
    void UpdateFacing(const Vector2& direction);
    void StopMovement();

    Vector2 GetForward() const;

    bool GetAnimationLock() const { return mIsAnimationLocked; }
    void SetAnimationLock(bool isLocked);

    int GetHP() const { return hp; }
    int SetHP(int newHP) { hp = std::clamp(newHP, 0, maxHp); return hp; }
    int GetMaxHP() const { return maxHp; }

    bool GetMovementLock() const { return mIsMovementLocked; }
    void SetMovementLock(bool isLocked);

    bool IsUsingSkill() const { return mIsUsingSkill; }
    void SetIsUsingSkill(bool isUsing) { mIsUsingSkill = isUsing; }

    virtual std::vector<SkillBase*> GetSkills() const { return mSkills; }
    
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
    int maxHp;

    std::vector<SkillBase*> mSkills;

    class RigidBodyComponent *mRigidBodyComponent;
    class AnimatorComponent *mAnimatorComponent;
    class ColliderComponent *mColliderComponent;
    class CollisionFilter mSkillFilter;

    float mForwardSpeed;
    float mSpeedMultiplier = 1.0f;

    bool mIsMoving;

    bool mIsAnimationLocked;
    bool mIsMovementLocked;

    bool mIsUsingSkill;

    bool mIsDead;
};