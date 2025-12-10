#include "Dummy.h"
#include "../../Game.h"
#include "../../GameConstants.h"
#include "../../Components/Drawing/AnimatorComponent.h"
#include "../../Components/Physics/RigidBodyComponent.h"
#include "../../Components/Physics/ColliderComponent.h"
#include "Character.h"

Dummy::Dummy(class Game* game, Vector2 position, float forwardSpeed)
    : Character(game, position, forwardSpeed)
{
    mAnimatorComponent = new AnimatorComponent(this, "DummyAnim", GameConstants::TILE_SIZE, GameConstants::TILE_SIZE);
    ((AABBCollider*)mColliderComponent->GetCollider())->SetSize(Vector2(GameConstants::TILE_SIZE, GameConstants::TILE_SIZE));
    ResetCollisionFilter();
}

Dummy::~Dummy()
{
}

void Dummy::TakeDamage(int damage)
{
	hp = Math::Max(hp - damage, 1);
	mAnimatorComponent->PlayAnimationOnce("Hit");
}