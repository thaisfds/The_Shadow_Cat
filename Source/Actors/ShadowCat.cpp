//
// Created by thais on 11/5/2025.
//

#include "ShadowCat.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Components/ParticleSystemComponent.h"

ShadowCat::ShadowCat(Game *game, const float forwardSpeed, const float jumpSpeed)
    : Actor(game), mIsRunning(false), mIsDead(false), mForwardSpeed(forwardSpeed), mJumpSpeed(jumpSpeed)
{
    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/ShadowCat/ShadowCat.png", "../Assets/Sprites/ShadowCat/ShadowCat.json", Game::TILE_SIZE, Game::TILE_SIZE);
    mRigidBodyComponent = new RigidBodyComponent(this);
    mColliderComponent = new AABBColliderComponent(this, 0, 0, Game::TILE_SIZE, Game::TILE_SIZE, ColliderLayer::Player);
    mRigidBodyComponent->SetApplyGravity(false);

    mDrawComponent->AddAnimation("Idle", {0});
    mDrawComponent->AddAnimation("Run", {0, 1, 0, 2});

    mDrawComponent->SetAnimation("Idle");
    mDrawComponent->SetAnimFPS(10.0f);
}

void ShadowCat::OnProcessInput(const uint8_t *state)
{
    if (mIsDead)
        return;

    Vector2 force = Vector2::Zero;
    Vector2 vel = mRigidBodyComponent->GetVelocity();
    mIsRunning = false;

    // Right (D key)
    if (state[SDL_SCANCODE_D])
    {
        mIsRunning = true;
        force.x += mForwardSpeed;
        SetScale(Vector2(1.0f, GetScale().y));
    }
    // Left (A key)
    else if (state[SDL_SCANCODE_A])
    {
        mIsRunning = true;
        force.x -= mForwardSpeed;
        SetScale(Vector2(-1.0f, GetScale().y));
    }
    else
    {
        vel.x = 0.0f;
    }

    // Down (S key)
    if (state[SDL_SCANCODE_S])
    {
        mIsRunning = true;
        force.y += mForwardSpeed;
    }
    // Up (W key)
    else if (state[SDL_SCANCODE_W])
    {
        mIsRunning = true;
        force.y -= mForwardSpeed;
    }
    else
    {
        vel.y = 0.0f;
    }

    if (mIsRunning)
    {
        mRigidBodyComponent->ApplyForce(force);
    }

    mRigidBodyComponent->SetVelocity(vel);
}

void ShadowCat::OnUpdate(float deltaTime)
{
    // Clamp position to level bounds
    Vector2 pos = GetPosition();
    const float margin = 15.0f;
    const float maxX = Game::LEVEL_WIDTH * Game::TILE_SIZE - margin;
    const float maxY = Game::LEVEL_HEIGHT * Game::TILE_SIZE - margin;

    if (pos.x < margin)
    {
        pos.x = margin;
        SetPosition(pos);
    }
    if (pos.x > maxX)
    {
        pos.x = maxX;
        SetPosition(pos);
    }
    if (pos.y < margin)
    {
        pos.y = margin;
        SetPosition(pos);
    }
    if (pos.y > maxY)
    {
        pos.y = maxY;
        SetPosition(pos);
    }

    ManageAnimations();
}

void ShadowCat::ManageAnimations()
{
    if (!mIsDead)
    {
        if (mIsRunning)
        {
            mDrawComponent->SetAnimation("Run");
        }
        else
        {
            mDrawComponent->SetAnimation("Idle");
        }
    }
}

void ShadowCat::Kill()
{
}

void ShadowCat::OnHorizontalCollision(const float minOverlap, AABBColliderComponent *other)
{
}

void ShadowCat::OnVerticalCollision(const float minOverlap, AABBColliderComponent *other)
{
}
