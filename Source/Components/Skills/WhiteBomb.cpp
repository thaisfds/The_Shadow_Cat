#include "WhiteBomb.h"
#include "../../Game.h"
#include "../../GameConstants.h"
#include "../../LevelManager.h"
#include "../Drawing/AnimatorComponent.h"
#include "../../Actors/Characters/Character.h"
#include "../Physics/ColliderComponent.h"
#include "../Physics/Physics.h"
#include "../../SkillFactory.h"
#include "../../Actors/Characters/ShadowCat.h"

WhiteBomb::WhiteBomb(Actor* owner, int updateOrder)
	: SkillBase(owner, updateOrder)
{
	LoadSkillDataFromJSON("WhiteBombData");

	float duration = mCharacter->GetComponent<AnimatorComponent>()->GetAnimationDuration("WhiteBomb");
	if (duration == 0.0f) duration = 1.0f;
	
	AddDelayedAction(0.5f, [this]() { Execute(); });
	AddDelayedAction(duration, [this]() { EndSkill(); });
}

nlohmann::json WhiteBomb::LoadSkillDataFromJSON(const std::string& fileName)
{
	auto data = SkillBase::LoadSkillDataFromJSON(fileName);

	mProjectileSpeed = GameJsonParser::GetFloatEffectValue(data, "projectileSpeed");
	mDamage = GameJsonParser::GetFloatEffectValue(data, "damage");
	mAreaOfEffect = GameJsonParser::GetAreaOfEffect(data);

	return data;
}

void WhiteBomb::Execute()
{
	float lifetime = mRange / mProjectileSpeed;

	// Play bomb sound
	mCharacter->GetGame()->GetAudio()->PlaySound("s05_furball_launch1.wav", false, 0.7f);

	LevelManager::Instance().GetWhiteBombActor()->Awake(
		mCharacter->GetPosition() + mTargetVector * 20.0f,
		mTargetVector,
		mProjectileSpeed,
		mDamage,
		mCharacter->GetSkillFilter(),
		mAreaOfEffect,
		lifetime
	);
}

void WhiteBomb::StartSkill(Vector2 targetPosition)
{
	SkillBase::StartSkill(targetPosition);
	mTargetVector -= mCharacter->GetPosition();
	mTargetVector.Normalize();

	mCharacter->GetComponent<AnimatorComponent>()->PlayAnimationOnce("WhiteBomb");
	mCharacter->SetMovementLock(true);
}

void WhiteBomb::EndSkill()
{
	SkillBase::EndSkill();

	mCharacter->SetMovementLock(false);
}

bool WhiteBomb::EnemyShouldUse()
{
	auto player = LevelManager::Instance().GetPlayer();
	if (!player) return false;

	Vector2 toPlayer = player->GetPosition() - mCharacter->GetPosition();
	float distanceToPlayer = toPlayer.Length();

	return distanceToPlayer <= mRange;
}

WhiteBombActor::WhiteBombActor(class Game* game)
	: Actor(game)
{
	mAnimatorComponent = new AnimatorComponent(this, "WhiteBombAnim", GameConstants::TILE_SIZE, GameConstants::TILE_SIZE);
	CollisionFilter filter;
	mColliderComponent = new ColliderComponent(this, 0, 0, nullptr, filter);
	mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 0.0f, false);
	Kill();
}

WhiteBombActor::~WhiteBombActor()
{
}

void WhiteBombActor::OnUpdate(float deltaTime)
{
	if (mDead) return;

	mLifetime -= deltaTime;
	if (mLifetime <= 0.0f)
	{
		Kill();
		return;
	}

	// Always follow the player
	auto player = LevelManager::Instance().GetPlayer();
	if (player && !player->IsDead())
	{
		Vector2 toPlayer = player->GetPosition() - GetPosition();
		float distance = toPlayer.Length();
		
		if (distance > 0.1f)
		{
			Vector2 direction = toPlayer;
			direction.Normalize();
			Vector2 velocity = direction * mSpeed;
			mRigidBodyComponent->SetVelocity(velocity);
		}
		else
		{
			// Hit player, deal damage
			player->TakeDamage(mDamage);
			Kill();
			return;
		}
	}
	else
	{
		// Player is dead or doesn't exist, continue in current direction
		Vector2 velocity = mDirection * mSpeed;
		mRigidBodyComponent->SetVelocity(velocity);
	}

	// Check for collisions
	ColliderComponent* colliderComp = GetComponent<ColliderComponent>();
	auto hitColliders = Physics::GetOverlappingColliders(GetGame(), colliderComp->GetCollider());
	for (auto collider : hitColliders)
	{
		auto enemyActor = collider->GetOwner();
		auto playerCharacter = dynamic_cast<ShadowCat*>(enemyActor);
		if (playerCharacter)
		{
			playerCharacter->TakeDamage(mDamage);
			Kill();
			return;
		}
	}

	mDelayedActions.Update(deltaTime);
}

void WhiteBombActor::Kill()
{
	mAnimatorComponent->SetVisible(false);
	mAnimatorComponent->SetEnabled(false);
	mColliderComponent->SetEnabled(false);
	mColliderComponent->SetCollider(nullptr);
	mColliderComponent->SetDebugDrawIfDisabled(false);
	mRigidBodyComponent->SetEnabled(false);
	mDead = true;

	mDelayedActions.Clear();
}

void WhiteBombActor::Awake(Vector2 position, Vector2 direction, float speed, int damage, CollisionFilter filter, Collider* areaOfEffect, float lifetime)
{
	mAnimatorComponent->SetEnabled(true);
	mAnimatorComponent->SetVisible(true);
	mColliderComponent->SetEnabled(true);
	mColliderComponent->SetFilter(filter);
	mColliderComponent->SetCollider(areaOfEffect);
	mColliderComponent->SetDebugDrawIfDisabled(true);
	mRigidBodyComponent->SetEnabled(true);

	int dim = ((CircleCollider*)areaOfEffect)->GetRadius() * 3.0f;
	mAnimatorComponent->SetSize(Vector2(dim, dim));
	
	SetPosition(position);
	mDirection = direction;
	mSpeed = speed;
	mDamage = damage;
	mLifetime = lifetime;
	mDead = false;
	
	mDelayedActions.Reset();
}

