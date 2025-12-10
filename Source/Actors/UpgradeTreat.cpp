#include "UpgradeTreat.h"
#include "../Components/Physics/ColliderComponent.h"
#include "../Components/Physics/Physics.h"
#include "Characters/ShadowCat.h"
#include "../GameConstants.h"

UpgradeTreat::UpgradeTreat(class Game* game)
	: Actor(game)
	, mCollected(true)
{
	CollisionFilter filter;
	filter.belongsTo = CollisionFilter::GroupMask({ CollisionGroup::Collectible });
	filter.collidesWith = CollisionFilter::GroupMask({ CollisionGroup::Player });

	Collider *collider = new AABBCollider(GameConstants::TILE_SIZE / 2, GameConstants::TILE_SIZE / 2);
	mColliderComponent = new ColliderComponent(this, Vector2::Zero, collider);
	mColliderComponent->SetFilter(filter);
	mColliderComponent->SetEnabled(false);

	mAnimatorComponent = new AnimatorComponent(this, "UpgradeTreatAnim", GameConstants::TILE_SIZE, GameConstants::TILE_SIZE);

	Kill();
}

void UpgradeTreat::Awake(const Vector2& position)
{
	mCollected = false;
	mPosition = position;

	mAnimatorComponent->SetEnabled(true);
	mAnimatorComponent->SetVisible(true);
}

void UpgradeTreat::Kill()
{
	mCollected = true;

	mAnimatorComponent->SetVisible(false);
	mAnimatorComponent->SetEnabled(false);
}

void UpgradeTreat::OnUpdate(float deltaTime)
{
	if (mCollected) return;

	auto player = mGame->GetPlayer();
	auto playerCollider = player->GetComponent<ColliderComponent>()->GetCollider();
	if (!mColliderComponent->GetCollider()->CheckCollision(playerCollider)) return;

	player->AddUpgradePoint();
	Kill();
}