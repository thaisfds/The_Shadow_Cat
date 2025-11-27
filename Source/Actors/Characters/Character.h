#pragma once
#include "../Actor.h"

class RigidBodyComponent;
class AnimatorComponent;
class AABBColliderComponent;

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

protected:
    void ManageAnimations();

    int hp;

    class RigidBodyComponent *mRigidBodyComponent;
    class AnimatorComponent *mAnimatorComponent;
    class AABBColliderComponent *mColliderComponent;

    float mForwardSpeed;

    bool mIsMoving;

    bool mIsAnimationLocked;
    bool mIsMovementLocked;

    bool mIsDead;
};