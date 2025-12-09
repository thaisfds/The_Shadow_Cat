//
// Created by thais on 11/5/2025.
//

#include "ShadowCat.h"
#include "../../Game.h"
#include "../../GameConstants.h"
#include "../../Components/Drawing/AnimatorComponent.h"
#include "../../Components/Physics/RigidBodyComponent.h"
#include "../../Components/Physics/ColliderComponent.h"
#include "../../Components/ParticleSystemComponent.h"
#include "../../Components/Skills/SkillInputHandler.h"
#include "Character.h"
#include <cmath>

ShadowCat::ShadowCat(Game *game, const float forwardSpeed)
    : Character(game, forwardSpeed)
{
    mAnimatorComponent = new AnimatorComponent(this, "ShadowCatAnim", GameConstants::TILE_SIZE, GameConstants::TILE_SIZE);
    mRigidBodyComponent = new RigidBodyComponent(this);
    
    Collider *collider = new AABBCollider(48, 32);
    mColliderComponent = new ColliderComponent(this, 0, 16, collider, GetBasePlayerFilter());
    
    mSkillInputHandler = new SkillInputHandler(this);
    mRigidBodyComponent->SetApplyGravity(false);

    // Setup animations
    mAnimatorComponent->AddAnimation("Idle", {0});
    mAnimatorComponent->AddAnimation("Run", {0, 1, 2, 3});
    mAnimatorComponent->AddAnimation("BasicAttack", {4, 5, 6, 7, 8, 0});
    
    mAnimatorComponent->LoopAnimation("Idle");

    hp = 10;
    maxHp = hp;
}

void ShadowCat::OnProcessInput(const uint8_t *state)
{
    if (mIsDead || mIsMovementLocked) return;

    Vector2 dir = Vector2::Zero;
    mIsMoving = false;

    // Keyboard input
    if (state[SDL_SCANCODE_D])
    {
        dir.x += 1.0f;
    }
    if (state[SDL_SCANCODE_A])
    {
        dir.x -= 1.0f;
    }
    if (state[SDL_SCANCODE_S])
    {
        dir.y += 1.0f;
    }
    if (state[SDL_SCANCODE_W])
    {
        dir.y -= 1.0f;
    }

    // Gamepad input
    SDL_GameController *controller = GetGame()->mController;
    if (controller)
    {
        // Left Stick (analog)
        int x_axis = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
        int y_axis = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);

        float x_dir = 0.0f;
        if (std::abs(x_axis) > GameConstants::JOYSTICK_DEAD_ZONE)
        {
            x_dir = (float)x_axis / 32767.0f;
        }

        float y_dir = 0.0f;
        if (std::abs(y_axis) > GameConstants::JOYSTICK_DEAD_ZONE)
        {
            y_dir = (float)y_axis / 32767.0f;
        }

        dir.x += x_dir;
        dir.y += y_dir;

        // D-Pad (arrows)
        if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_UP))
        {
            dir.y -= 1.0f;
        }
        if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN))
        {
            dir.y += 1.0f;
        }
        if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT))
        {
            dir.x -= 1.0f;
        }
        if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT))
        {
            dir.x += 1.0f;
        }
    }

    Vector2 desiredVel = Vector2::Zero;
    if (dir.LengthSq() > 0.0f)
    {
        mIsMoving = true;
        dir.Normalize();
        desiredVel = dir * mForwardSpeed;

        if (dir.x > 0.0f)
        {
            SetScale(Vector2(1.0f, GetScale().y));
        }
        else if (dir.x < 0.0f)
        {
            SetScale(Vector2(-1.0f, GetScale().y));
        }
    }

    mRigidBodyComponent->SetVelocity(desiredVel);
}

void ShadowCat::OnHandleEvent(const SDL_Event& event)
{
    if (mSkillInputHandler) mSkillInputHandler->HandleEvent(event);
}

void ShadowCat::OnUpdate(float deltaTime)
{
    Character::OnUpdate(deltaTime);
}

void ShadowCat::TakeDamage(int damage)
{
    hp -= damage;

    if (hp <= 0) Kill();
}
void ShadowCat::Kill()
{
    mGame->SetGameOver(true);
}