#pragma once

#include "Drawing/AnimatorComponent.h"
#include "../Actors/Actor.h"
#include "Component.h"

class AnimatedParticle : public Actor
{
public:
	AnimatedParticle(class Game *game, bool hasCollider, std::string particleAnimName);

	void OnUpdate(float deltaTime) override;

	bool IsDead() const { return mIsDead; }
	void Awake(const Vector2 &position, float rotation, float lifetime = 1.0f, bool flipV = false);
	void Kill() override;
private:
	float mLifeTime;
	bool mIsDead;

	class AnimatorComponent *mAnimatorComponent;
	class RigidBodyComponent *mRigidBodyComponent;
	class ColliderComponent *mColliderComponent;
};

class AnimatedParticleSystemComponent : public Component
{
public:
	AnimatedParticleSystemComponent(class Actor *owner, std::string particlePath, bool hasCollider = true, int poolSize = 100, int updateOrder = 10);
	void EmitParticle(float lifetime, float speed, const Vector2 &offsetPosition = Vector2::Zero);
    void EmitParticleAt(float lifetime, float speed, const Vector2 &position, const float rotation, bool flipV = false);
	
private:
	std::vector<class AnimatedParticle *> mParticles;
};