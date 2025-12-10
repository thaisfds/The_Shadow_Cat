#include "FurBall.h"
#include "../../Game.h"
#include "../../GameConstants.h"
#include "../Drawing/AnimatorComponent.h"
#include "../../Actors/Characters/Character.h"
#include "../Physics/ColliderComponent.h"
#include "../Physics/Physics.h"
#include "../../SkillFactory.h"

FurBall::FurBall(Actor* owner, int updateOrder)
	: SkillBase(owner, updateOrder)
{
	LoadSkillDataFromJSON("FurBallData");

	float duration = mCharacter->GetComponent<AnimatorComponent>()->GetAnimationDuration("FurBall");
	if (duration == 0.0f) duration = 1.0f;
	
	AddDelayedAction(0.5f, [this]() { Execute(); });
	AddDelayedAction(duration, [this]() { EndSkill(); });
}

nlohmann::json FurBall::LoadSkillDataFromJSON(const std::string& fileName)
{
	auto data = SkillBase::LoadSkillDataFromJSON(fileName);

	mProjectileSpeed = GameJsonParser::GetFloatEffectValue(data, "projectileSpeed");
	mDamage = GameJsonParser::GetFloatEffectValue(data, "damage");
	mAreaOfEffect = GameJsonParser::GetAreaOfEffect(data);
	auto id = GameJsonParser::GetStringValue(data, "id");
	SkillFactory::Instance().RegisterSkill(id, [](Actor* owner) { return new FurBall(owner); });

	mUpgrades.push_back(GameJsonParser::GetUpgradeInfo(data, "projectileSpeed", &mProjectileSpeed));
	mUpgrades.push_back(GameJsonParser::GetUpgradeInfo(data, "damage", &mDamage));
	mUpgrades.push_back(GameJsonParser::GetUpgradeInfo(data, "cooldown", &mCooldown));
	mUpgrades.push_back(GameJsonParser::GetUpgradeInfo(data, "range", &mRange));

	return data;
}

void FurBall::Execute()
{
	float lifetime = mRange / mProjectileSpeed;

	// Play furball sound
	mCharacter->GetGame()->GetAudio()->PlaySound("s05_furball_launch1.wav", false, 0.7f);

	mCharacter->GetGame()->GetFurBallActor()->Awake(
		mCharacter->GetPosition() + mTargetVector * 20.0f,
		mTargetVector,
		mProjectileSpeed,
		mDamage,
		mCharacter->GetSkillFilter(),
		mAreaOfEffect,
		lifetime
	);
}

void FurBall::StartSkill(Vector2 targetPosition)
{
	SkillBase::StartSkill(targetPosition);
	mTargetVector -= mCharacter->GetPosition();
	mTargetVector.Normalize();

	mCharacter->GetComponent<AnimatorComponent>()->PlayAnimationOnce("FurBall");
	mCharacter->SetMovementLock(true);
}

void FurBall::EndSkill()
{
	SkillBase::EndSkill();

	mCharacter->SetMovementLock(false);
}

FurBallActor::FurBallActor(class Game* game)
	: Actor(game)
{
	mAnimatorComponent = new AnimatorComponent(this, "FurBallAnim", GameConstants::TILE_SIZE, GameConstants::TILE_SIZE);
	CollisionFilter filter;
	mColliderComponent = new ColliderComponent(this, 0, 0, nullptr, filter);
	mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 0.0f, false);
	Kill();
}

FurBallActor::~FurBallActor()
{
}

void FurBallActor::OnUpdate(float deltaTime)
{
	if (mDead) return;

	Vector2 velocity = mDirection * mSpeed;
	mRigidBodyComponent->SetVelocity(velocity);

	// Check for collisions
	ColliderComponent* colliderComp = GetComponent<ColliderComponent>();
	auto hitColliders = Physics::GetOverlappingColliders(GetGame(), colliderComp->GetCollider());
	for (auto collider : hitColliders)
	{
		auto enemyActor = collider->GetOwner();
		auto enemyCharacter = dynamic_cast<Character*>(enemyActor);
		if (enemyCharacter)
		{
			enemyCharacter->TakeDamage(mDamage);
			Kill();
			return;
		}
	}

	mDelayedActions.Update(deltaTime);
}

void FurBallActor::Kill()
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

void FurBallActor::Awake(Vector2 position, Vector2 direction, float speed, int damage, CollisionFilter filter, Collider* areaOfEffect, float lifetime)
{
	mAnimatorComponent->SetEnabled(true);
	mAnimatorComponent->SetVisible(true);
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
	mDead = false;
	
	mDelayedActions.Reset();
	AddDelayedAction(lifetime, [this]() { Kill(); });
}