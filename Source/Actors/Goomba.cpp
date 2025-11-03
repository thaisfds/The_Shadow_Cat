#include "Goomba.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

Goomba::Goomba(Game *game, float forwardSpeed, float deathTime)
    : Actor(game), mDyingTimer(deathTime), mIsDying(false), mForwardSpeed(forwardSpeed)
{
}

void Goomba::Kill()
{
}

void Goomba::OnUpdate(float deltaTime)
{
}

void Goomba::OnHorizontalCollision(const float minOverlap, AABBColliderComponent *other)
{
}

void Goomba::OnVerticalCollision(const float minOverlap, AABBColliderComponent *other)
{
}
