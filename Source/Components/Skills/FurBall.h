#pragma once

#include "SkillBase.h"
#include "../../Math.h"
#include "../../Actors/Actor.h"
#include "../../Components/Physics/ColliderComponent.h"
#include "../../Components/Physics/RigidBodyComponent.h"
#include "../Physics/CollisionFilter.h"

class FurBall : public SkillBase
{
public:
	FurBall(Actor* owner, int updateOrder = 100);

	void Update(float deltaTime) override;

	bool CanUse() const override { return SkillBase::CanUse(); }

	void Execute() override;
	void EndAttack();

private:
	float mProjectileSpeed;
	int mDamage;

	float mDelay;
	float mTimer;
	float mDuration;
	bool mIsAttacking;
	bool mFired;
	Vector2 mDirection;
};

class FurBallActor : public Actor
{
public:
	FurBallActor(class Game* game);
	~FurBallActor();

	void OnUpdate(float deltaTime) override;

	void Kill() override;
	void Awake(Vector2 position, Vector2 direction, float speed, int damage, float delay, CollisionFilter filter);

	bool IsDead() const { return mDead; }

private:
	bool mDead;

	float mSpeed;
	int mDamage;
    Vector2 mDirection;

	class AnimatorComponent *mAnimatorComponent;
	class ColliderComponent *mColliderComponent;
	class RigidBodyComponent *mRigidBodyComponent;
};