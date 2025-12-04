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
	mCooldown = 5.0f;
	mCurrentCooldown = 0.0f;

	mDamage = 15;
	mStompRadius = 50.0f;
}

void Stomp::StartSkill(Vector2 targetPosition)
{
	mCurrentCooldown = mCooldown;

	CollisionFilter filter;
	filter.belongsTo = CollisionFilter::GroupMask({ CollisionGroup::PlayerSkills });
	filter.collidesWith = CollisionFilter::GroupMask({ CollisionGroup::Enemy });

	mCharacter->GetGame()->GetStompActor()->Awake(
		mCharacter->GetGame()->GetMouseWorldPosition(),
		mDamage,
		0.65f,
		filter
	);
}

StompActor::StompActor(class Game* game)
	: Actor(game)
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
	mDelayedActions.Update(deltaTime);
}

void StompActor::Execute()
{
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

void StompActor::Kill()
{
	mAnimatorComponent->SetVisible(false);
	mAnimatorComponent->SetEnabled(false);
	mColliderComponent->SetEnabled(false);
	mDead = true;

	mDelayedActions.Clear();
}

void StompActor::Awake(Vector2 position, int damage, float delay, CollisionFilter filter)
{
	mAnimatorComponent->SetEnabled(true);
	mAnimatorComponent->SetVisible(true);
	mAnimatorComponent->PlayAnimationOnce("Stomp");
	mColliderComponent->SetFilter(filter);
	
	mDamage = damage;
	mDead = false;
	SetPosition(position);

	mDelayedActions.Reset();
	
	float stompLifetime = GetComponent<AnimatorComponent>()->GetAnimationDuration("Stomp");
	AddDelayedAction(delay, [this]() { Execute(); });
	AddDelayedAction(stompLifetime, [this]() { Kill(); });
}