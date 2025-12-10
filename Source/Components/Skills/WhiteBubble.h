#pragma once

#include "SkillBase.h"
#include "../../Math.h"
#include "../../Actors/Actor.h"
#include "../../Components/Physics/ColliderComponent.h"
#include "../../Components/Physics/RigidBodyComponent.h"
#include "../Physics/CollisionFilter.h"
#include "../../DelayedActionSystem.h"

class WhiteBubble : public SkillBase
{
public:
	WhiteBubble(Actor* owner, int updateOrder = 100);

	void StartSkill(Vector2 targetPosition) override;
	void EndSkill() override;

	void Execute();

	bool EnemyShouldUse() override;

private:
	int mDamage;
	float mDuration; // 20 seconds
	Collider* mAreaOfEffect;

	nlohmann::json LoadSkillDataFromJSON(const std::string& fileName) override;
};

class WhiteBubbleActor : public Actor
{
public:
	WhiteBubbleActor(class Game* game);
	~WhiteBubbleActor();

	void OnUpdate(float deltaTime) override;

	void Kill() override;
	void Awake(Vector2 position, int damage, CollisionFilter filter, Collider* areaOfEffect, float duration);

	bool IsDead() const { return mDead; }

private:
	bool mDead;

	int mDamage;
	float mDuration;
	float mDamageCooldown; // Cooldown between damage ticks
	float mLastDamageTime; // Last time damage was dealt

	class DelayedActionSystem mDelayedActions;

	class AnimatorComponent *mAnimatorComponent;
	class ColliderComponent *mColliderComponent;

	void AddDelayedAction(float delay, std::function<void()> action)
	{
		mDelayedActions.AddDelayedAction(delay, action);
	}
};

