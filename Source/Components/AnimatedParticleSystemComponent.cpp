#include "AnimatedParticleSystemComponent.h"
#include "../GameConstants.h"
#include "Physics/Collider.h"
#include "Physics/ColliderComponent.h"
#include "Physics/CollisionFilter.h"
#include "Physics/RigidBodyComponent.h"


AnimatedParticle::AnimatedParticle(class Game *game, bool hasCollider, std::string particleAnimName)
	: Actor(game), mAnimatorComponent(nullptr), mRigidBodyComponent(nullptr), mColliderComponent(nullptr), mIsDead(true), mLifeTime(1.0f)
{
	mAnimatorComponent = new AnimatorComponent(this, particleAnimName, GameConstants::TILE_SIZE, GameConstants::TILE_SIZE);
	mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 0.0f, true);

	if (hasCollider)
	{
		CollisionFilter filter;
		filter.belongsTo = CollisionFilter::GroupMask({CollisionGroup::Environment});
		filter.collidesWith = CollisionFilter::GroupMask({CollisionGroup::Player, CollisionGroup::Enemy, CollisionGroup::Environment});

		Collider *collider = new AABBCollider(GameConstants::TILE_SIZE, GameConstants::TILE_SIZE);
		mColliderComponent = new ColliderComponent(this, 0, 0, collider, filter);
	} 

	SetState(ActorState::Paused);
	mAnimatorComponent->SetVisible(false);
}

void AnimatedParticle::Kill()
{
	mIsDead = true;
	SetState(ActorState::Paused);
	mAnimatorComponent->SetVisible(false);
	if (mColliderComponent) mColliderComponent->SetEnabled(false);

	// Reset velocity
	mRigidBodyComponent->SetVelocity(Vector2::Zero);
}

void AnimatedParticle::Awake(const Vector2 &position, float rotation, float lifetime, bool flipV)
{
	mLifeTime = lifetime;

	mIsDead = false;
	SetState(ActorState::Active);
	mAnimatorComponent->SetVisible(true);
	mAnimatorComponent->ResetAnimation();
	mAnimatorComponent->SetAnimSpeed(mAnimatorComponent->GetCurrentAnimationDuration() / lifetime); // Adjust animation speed to match lifetime
	if (mColliderComponent) mColliderComponent->SetEnabled(true);

	if (flipV)
		SetScale(Vector2(GetScale().x, -1.0f));
	else
		SetScale(Vector2(GetScale().x, 1.0f));

	SetPosition(position);
	SetRotation(rotation);
}

void AnimatedParticle::OnUpdate(float deltaTime)
{
	mLifeTime -= deltaTime;
	if (mLifeTime <= 0)
	{
		Kill();
		return;
	}
}

AnimatedParticleSystemComponent::AnimatedParticleSystemComponent(class Actor *owner, std::string particlePath, bool hasCollider, int poolSize, int updateOrder)
	: Component(owner, updateOrder)
{
	// Create a pool of particles
	for (int i = 0; i < poolSize; i++)
	{
		auto *p = new AnimatedParticle(owner->GetGame(), hasCollider, particlePath);
		mParticles.push_back(p);
	}
}

void AnimatedParticleSystemComponent::EmitParticle(float lifetime, float speed, const Vector2 &offsetPosition)
{
    for (auto p : mParticles)
    {
        if (p->IsDead())
        {
            // Wake up the particle
            Vector2 spawnPos = mOwner->GetPosition() + offsetPosition * mOwner->GetScale().x;
            p->Awake(spawnPos, mOwner->GetRotation(), lifetime);

            // Forward velocity
            Vector2 direction = mOwner->GetScale();
            p->GetComponent<RigidBodyComponent>()->ApplyForce(direction * speed);

            // Break inner loop to emit only one particle per iteration
            break;
        }
    }
}

void AnimatedParticleSystemComponent::EmitParticleAt(float lifetime, float speed, const Vector2 &position, const float rotation, bool flipV)
{
	for (auto p : mParticles)
	{
		if (p->IsDead())
		{
			// Wake up the particle
			p->Awake(position, rotation, lifetime, flipV);

			// Forward velocity
			Vector2 direction = Vector2(std::cos(rotation), std::sin(rotation));
			p->GetComponent<RigidBodyComponent>()->ApplyForce(direction * speed);

			// Break inner loop to emit only one particle per iteration
			break;
		}
	}
    
}