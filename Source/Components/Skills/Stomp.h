#pragma once

#include "SkillBase.h"
#include "../../Math.h"
#include "../../Actors/Actor.h"
#include "../Physics/CollisionFilter.h"
#include "../../DelayedActionSystem.h"

class Stomp : public SkillBase
{
public:
	Stomp(Actor* owner, int updateOrder = 100);

	void StartSkill(Vector2 targetPosition) override;
	
private:
	int mDamage;
	float mStompRadius;

	float mStompDelay;
	float mStompTimer;
};

class StompActor : public Actor
{
public:
	StompActor(class Game* game);
	~StompActor();

	void OnUpdate(float deltaTime) override;

	void Execute();

	void Kill() override;
	void Awake(Vector2 position, int damage, float delay, CollisionFilter filter);

	bool IsDead() const { return mDead; }
private:
	bool mDead;

	int mDamage;

	class DelayedActionSystem mDelayedActions;
	
	class AnimatorComponent *mAnimatorComponent;
    class ColliderComponent *mColliderComponent;

	void AddDelayedAction(float delay, std::function<void()> action)
	{
		mDelayedActions.AddDelayedAction(delay, action);
	}
};