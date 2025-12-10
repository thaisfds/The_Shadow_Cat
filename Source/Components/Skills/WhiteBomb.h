#pragma once

#include "SkillBase.h"
#include "../../Math.h"
#include "../../Actors/Actor.h"
#include "../../Components/Physics/ColliderComponent.h"
#include "../../Components/Physics/RigidBodyComponent.h"
#include "../Physics/CollisionFilter.h"
#include "../../DelayedActionSystem.h"

class WhiteBomb : public SkillBase
{
public:
	WhiteBomb(Actor* owner, int updateOrder = 100);

	void StartSkill(Vector2 targetPosition) override;
	void EndSkill() override;

	void Execute();

private:
	float mProjectileSpeed;
	float mDamage;
	Collider* mAreaOfEffect;

	nlohmann::json LoadSkillDataFromJSON(const std::string& fileName) override;
};

class WhiteBombActor : public Actor
{
public:
	WhiteBombActor(class Game* game);
	~WhiteBombActor();

	void OnUpdate(float deltaTime) override;

	void Kill() override;
	void Awake(Vector2 position, Vector2 direction, float speed, int damage, CollisionFilter filter, Collider* areaOfEffect, float lifetime);

	bool IsDead() const { return mDead; }

private:
	bool mDead;

	float mSpeed;
	int mDamage;
    Vector2 mDirection;
	float mLifetime;

	class DelayedActionSystem mDelayedActions;

	class AnimatorComponent *mAnimatorComponent;
	class ColliderComponent *mColliderComponent;
	class RigidBodyComponent *mRigidBodyComponent;

	void AddDelayedAction(float delay, std::function<void()> action)
	{
		mDelayedActions.AddDelayedAction(delay, action);
	}
};

