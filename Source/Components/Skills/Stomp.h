#pragma once

#include "SkillBase.h"
#include "../../Math.h"
#include "../../Actors/Actor.h"
#include "../Physics/CollisionFilter.h"

class Stomp : public SkillBase
{
public:
	Stomp(Actor* owner, int updateOrder = 100);

	void Update(float deltaTime) override;

	void Execute(Vector2 targetPosition) override;
private:
	int mDamage;
	float mStompRadius;

	float mStompDelay;
	float mStompTimer;
	Vector2 mPosition;
};

class StompActor : public Actor
{
public:
	StompActor(class Game* game);
	~StompActor();

	void OnUpdate(float deltaTime) override;

	void Kill() override;
	void Awake(Vector2 position, int damage, float delay, CollisionFilter filter);

	bool IsDead() const { return mDead; }
private:
	bool mDead;

	float mStompLifetime;
	float mStompTimer;
	float mStompDelay;
	bool mHasDamaged;
	int mDamage;
	
	class AnimatorComponent *mAnimatorComponent;
    class ColliderComponent *mColliderComponent;
};