#include "FurBall.h"
#include "../../Game.h"
#include "../../GameConstants.h"
#include "../Drawing/AnimatorComponent.h"
#include "../../Actors/Characters/Character.h"
#include "../Physics/ColliderComponent.h"
#include "../Physics/Physics.h"

FurBall::FurBall(Actor* owner, int updateOrder)
	: SkillBase(owner, updateOrder)
{
	mName = "Fur Ball";
	mDescription = "Launch a ball of fur that damages enemies on impact.";
	mCooldown = 0.0f;
	mCurrentCooldown = 0.0f;
	mProjectileSpeed = 300.0f;
	mDamage = 20;
	mDelay = 0.5f;
	mTimer = 0.0f;

	mDuration = mCharacter->GetComponent<AnimatorComponent>()->GetAnimationDuration("FurBall");
	if (mDuration == 0.0f) mDuration = 1.0f;
}

void FurBall::Update(float deltaTime)
{
	SkillBase::Update(deltaTime);

	if (!mIsAttacking) return;

	mTimer += deltaTime;
	if (mTimer >= mDelay && !mFired)
	{
		mFired = true;
		
		CollisionFilter filter;
		filter.belongsTo = CollisionFilter::GroupMask({ CollisionGroup::PlayerSkills });
		filter.collidesWith = CollisionFilter::GroupMask({ CollisionGroup::Enemy });

		mCharacter->GetGame()->GetFurBallActor()->Awake(
			mCharacter->GetPosition() + mDirection * 20.0f,
			mDirection,
			mProjectileSpeed,
			mDamage,
			mDelay,
			filter
		);
	}

	if (mTimer >= mDuration) EndAttack();
}

void FurBall::Execute()
{
	mCharacter->GetComponent<AnimatorComponent>()->PlayAnimationOnce("FurBall");
	mCharacter->SetMovementLock(true);

	mIsAttacking = true;
	mTimer = 0.0f;
	mFired = false;

	Vector2 mouseWorldPos = mCharacter->GetGame()->GetMouseWorldPosition();
	mDirection = mouseWorldPos - mCharacter->GetPosition();
	mDirection.Normalize();

	StartCooldown();
}

void FurBall::EndAttack()
{
	mIsAttacking = false;
	mCharacter->SetMovementLock(false);
}

FurBallActor::FurBallActor(class Game* game)
	: Actor(game)
{
	mAnimatorComponent = new AnimatorComponent(this, "FurBallAnim", GameConstants::TILE_SIZE, GameConstants::TILE_SIZE);
	CollisionFilter filter;
	Collider *collider = new AABBCollider(GameConstants::TILE_SIZE / 2, GameConstants::TILE_SIZE / 2);
	mColliderComponent = new ColliderComponent(this, 0, 0, collider, filter);
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
}

void FurBallActor::Kill()
{
	mAnimatorComponent->SetVisible(false);
	mAnimatorComponent->SetEnabled(false);
	mColliderComponent->SetEnabled(false);
	mRigidBodyComponent->SetEnabled(false);
	mDead = true;
}

void FurBallActor::Awake(Vector2 position, Vector2 direction, float speed, int damage, float delay, CollisionFilter filter)
{
	mAnimatorComponent->SetEnabled(true);
	mAnimatorComponent->SetVisible(true);
	mColliderComponent->SetFilter(filter);
	mRigidBodyComponent->SetEnabled(true);

	SetPosition(position);
	mDirection = direction;
	mSpeed = speed;
	mDamage = damage;
	mDead = false;
}