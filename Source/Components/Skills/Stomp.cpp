#include "Stomp.h"

#include "../Drawing/AnimatorComponent.h"
#include "../Physics/CollisionFilter.h"
#include "../Physics/Physics.h"
#include "../../Actors/Characters/Character.h"
#include "../../Game.h"
#include "../../GameConstants.h"
#include "../Physics/ColliderComponent.h"


Stomp::Stomp(Actor* owner, int updateOrder)
	: SkillBase(owner, updateOrder)
{
	mName = "Stomp";
	mDescription = "Bring a pawerful stomp down, damaging nearby enemies.";
	mCooldown = 0.0f;
	mCurrentCooldown = 0.0f;

	mDamage = 30;
	mStompRadius = 50.0f;
}

void Stomp::Update(float deltaTime)
{
	SkillBase::Update(deltaTime);
}

void Stomp::Execute()
{
	CollisionFilter filter;
	filter.belongsTo = CollisionFilter::GroupMask({ CollisionGroup::PlayerSkills });
	filter.collidesWith = CollisionFilter::GroupMask({ CollisionGroup::Enemy });

	mCharacter->GetGame()->GetStompActor()->Awake(
		mCharacter->GetGame()->GetMouseWorldPosition(),
		mDamage,
		0.65f,
		filter
	);

	StartCooldown();
}

StompActor::StompActor(class Game* game)
	: Actor(game)
	, mStompTimer(0.0f)
	, mStompDelay(0.0f)
	, mDamage(0)
{
	mAnimatorComponent = new AnimatorComponent(this, "StompAnim", GameConstants::TILE_SIZE, GameConstants::TILE_SIZE);
	CollisionFilter filter;
	Collider *collider = new AABBCollider(GameConstants::TILE_SIZE, GameConstants::TILE_SIZE);
	mColliderComponent = new ColliderComponent(this, 0, 0, collider, filter);
	Kill();
}

StompActor::~StompActor()
{
}

void StompActor::OnUpdate(float deltaTime)
{
	if (mDead) return;

	mStompTimer += deltaTime;
	if (mStompTimer >= mStompDelay && !mHasDamaged)
	{
		mHasDamaged = true;

		ColliderComponent* colliderComp = GetComponent<ColliderComponent>();
		Vector2 position = GetPosition();

		auto hitColliders = Physics::GetOverlappingColliders(GetGame(), colliderComp->GetCollider());
		for (auto collider : hitColliders)
		{
			auto enemyActor = collider->GetOwner();
			auto enemyCharacter = dynamic_cast<Character*>(enemyActor);
			enemyCharacter->TakeDamage(mDamage);
		}
	}

	if (mStompTimer >= mStompLifetime) Kill();
}

void StompActor::Kill()
{
	mAnimatorComponent->SetVisible(false);
	mAnimatorComponent->SetEnabled(false);
	mColliderComponent->SetEnabled(false);
	mDead = true;
}

void StompActor::Awake(Vector2 position, int damage, float delay, CollisionFilter filter)
{
	mAnimatorComponent->SetEnabled(true);
	mAnimatorComponent->SetVisible(true);
	mAnimatorComponent->PlayAnimationOnce("Stomp");
	mColliderComponent->SetFilter(filter);
	
	mStompLifetime = GetComponent<AnimatorComponent>()->GetAnimationDuration("Stomp");
	mStompTimer = 0.0f;
	mStompDelay = delay;
	mHasDamaged = false;
	mDamage = damage;
	mDead = false;
	SetPosition(position);
}