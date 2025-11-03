#include "Mario.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Components/ParticleSystemComponent.h"

Mario::Mario(Game *game, const float forwardSpeed, const float jumpSpeed)
    : Actor(game), mIsRunning(false), mIsDead(false), mForwardSpeed(forwardSpeed), mJumpSpeed(jumpSpeed)
{
    new AnimatorComponent(this, "../Assets/Sprites/Mario/Idle.png", "", Game::TILE_SIZE, Game::TILE_SIZE);
}

void Mario::OnProcessInput(const uint8_t *state)
{
}

void Mario::OnUpdate(float deltaTime)
{

    ManageAnimations();
}

void Mario::ManageAnimations()
{
}

void Mario::Kill()
{
}

void Mario::OnHorizontalCollision(const float minOverlap, AABBColliderComponent *other)
{
}

void Mario::OnVerticalCollision(const float minOverlap, AABBColliderComponent *other)
{
}
