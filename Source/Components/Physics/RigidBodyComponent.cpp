//
// Created by Lucas N. Ferreira on 08/09/23.
//

#include <SDL.h>
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../GameConstants.h"
#include "RigidBodyComponent.h"
#include "ColliderComponent.h"

RigidBodyComponent::RigidBodyComponent(class Actor* owner, float mass, float friction, bool applyGravity, int updateOrder)
        :Component(owner, updateOrder)
        ,mMass(mass)
        ,mApplyGravity(applyGravity)
        ,mFrictionCoefficient(friction)
        ,mVelocity(Vector2::Zero)
        ,mAcceleration(Vector2::Zero)
{

}

void RigidBodyComponent::ApplyForce(const Vector2 &force)
{
    mAcceleration += force * (1.f/mMass);
}

void RigidBodyComponent::Update(float deltaTime)
{
    // Apply gravity acceleration
    if(mApplyGravity)
    {
        ApplyForce(Vector2::UnitY * GameConstants::GRAVITY);
    }

    // Apply friction
    if(Math::Abs(mVelocity.x) > 0.05f && mFrictionCoefficient != 0.0f)
    {
        ApplyForce(Vector2::UnitX * -mFrictionCoefficient * mVelocity.x);
    }

    // Euler Integration
    mVelocity += mAcceleration * deltaTime;

    mVelocity.x = Math::Clamp<float>(mVelocity.x, -GameConstants::MAX_SPEED_X, GameConstants::MAX_SPEED_X);
    mVelocity.y = Math::Clamp<float>(mVelocity.y, -GameConstants::MAX_SPEED_Y, GameConstants::MAX_SPEED_Y);

    if(Math::NearlyZero(mVelocity.x, 1.0f))
    {
        mVelocity.x = 0.f;
    }

    auto collider = mOwner->GetComponent<ColliderComponent>();

    Vector2 pos = mOwner->GetPosition();
    pos.x += mVelocity.x * deltaTime;
    pos.y += mVelocity.y * deltaTime;

    mOwner->SetPosition(pos);

    if (collider) collider->DetectCollisions(this);

    mAcceleration.Set(0.f, 0.f);
}