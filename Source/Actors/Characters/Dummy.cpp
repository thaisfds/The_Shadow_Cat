#include "Dummy.h"
#include "../../Game.h"
#include "../../Components/Drawing/AnimatorComponent.h"
#include "../../Components/Physics/RigidBodyComponent.h"
#include "../../Components/Physics/AABBColliderComponent.h"

Dummy::Dummy(class Game* game, float forwardSpeed)
    : Character(game, forwardSpeed)
{
    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/Dummy/Dummy.png", "../Assets/Sprites/Dummy/Dummy.json", Game::TILE_SIZE, Game::TILE_SIZE);
    mRigidBodyComponent = new RigidBodyComponent(this);
    mColliderComponent = new AABBColliderComponent(this, 0, 0, Game::TILE_SIZE, Game::TILE_SIZE, ColliderLayer::Enemy);
    mRigidBodyComponent->SetApplyGravity(false);

    mDrawComponent->AddAnimation("Idle", {0});
    mDrawComponent->AddAnimation("Hit", {1, 2, 2, 0});

    mDrawComponent->SetAnimation("Idle");
    mDrawComponent->SetAnimFPS(10.0f);
}

Dummy::~Dummy()
{
}