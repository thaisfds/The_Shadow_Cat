#include "Dummy.h"
#include "../../Game.h"
#include "../../GameConstants.h"
#include "../../Components/Drawing/AnimatorComponent.h"
#include "../../Components/Physics/RigidBodyComponent.h"
#include "../../Components/Physics/AABBColliderComponent.h"
#include "Character.h"

Dummy::Dummy(class Game* game, float forwardSpeed)
    : Character(game, forwardSpeed)
{
    mAnimatorComponent = new AnimatorComponent(this, "DummyAnim", GameConstants::TILE_SIZE, GameConstants::TILE_SIZE);
    mRigidBodyComponent = new RigidBodyComponent(this);
    mColliderComponent = new AABBColliderComponent(this, 0, 0, GameConstants::TILE_SIZE, GameConstants::TILE_SIZE, GetBaseEnemyFilter());
    mRigidBodyComponent->SetApplyGravity(false);
}

Dummy::~Dummy()
{
}

void Dummy::TakeDamage(int damage)
{
	hp = Math::Max(hp - damage, 1);
	mAnimatorComponent->PlayAnimationOnce("Hit");
}