#include "Dummy.h"
#include "../../Game.h"
#include "../../GameConstants.h"
#include "../../Components/Drawing/AnimatorComponent.h"
#include "../../Components/Physics/RigidBodyComponent.h"
#include "../../Components/Physics/AABBColliderComponent.h"

Dummy::Dummy(class Game* game, float forwardSpeed)
    : Character(game, forwardSpeed)
{
    mAnimatorComponent = new AnimatorComponent(this, "../Assets/Sprites/Dummy/Dummy.png", "../Assets/Sprites/Dummy/Dummy.json", GameConstants::TILE_SIZE, GameConstants::TILE_SIZE);
    mRigidBodyComponent = new RigidBodyComponent(this);
    mColliderComponent = new AABBColliderComponent(this, 0, 0, GameConstants::TILE_SIZE, GameConstants::TILE_SIZE, ColliderLayer::Enemy);
    mRigidBodyComponent->SetApplyGravity(false);

    mAnimatorComponent->AddAnimation("Idle", {0});
    mAnimatorComponent->AddAnimation("Hit", {1, 2, 0});

    mAnimatorComponent->LoopAnimation("Idle");
}

Dummy::~Dummy()
{
}

void Dummy::TakeDamage(int damage)
{
	hp = Math::Max(hp - damage, 1);
	mAnimatorComponent->PlayAnimationOnce("Hit");
}