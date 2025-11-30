#include "Dummy.h"
#include "../../Game.h"
#include "../../GameConstants.h"
#include "../../Components/Drawing/AnimatorComponent.h"
#include "../../Components/Physics/RigidBodyComponent.h"
#include "../../Components/Physics/ColliderComponent.h"
#include "Character.h"

Dummy::Dummy(class Game* game, float forwardSpeed)
    : Character(game, forwardSpeed)
{
    mAnimatorComponent = new AnimatorComponent(this, "DummyAnim", GameConstants::TILE_SIZE, GameConstants::TILE_SIZE);
    mRigidBodyComponent = new RigidBodyComponent(this);
    
    Collider *collider = new AABBCollider(GameConstants::TILE_SIZE, GameConstants::TILE_SIZE);
    mColliderComponent = new ColliderComponent(this, 0, 0, collider, GetBaseEnemyFilter());
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