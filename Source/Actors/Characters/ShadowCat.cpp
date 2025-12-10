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
#include <random>
#include <algorithm>

ShadowCat::ShadowCat(Game *game, Vector2 position, const float forwardSpeed)
    : Character(game, position, forwardSpeed)
    , mFootstepTimer(0.0f)
{
    mAnimatorComponent = new AnimatorComponent(this, "ShadowCatAnim", GameConstants::TILE_SIZE, GameConstants::TILE_SIZE);
    
    ResetCollisionFilter();

    mSkillFilter.belongsTo = CollisionFilter::GroupMask({CollisionGroup::PlayerSkills});
    mSkillFilter.collidesWith = CollisionFilter::GroupMask({CollisionGroup::Enemy});
    
    mSkillInputHandler = new SkillInputHandler(this);

    hp = 200;
    maxHp = hp;
}

std::vector<SkillBase*> ShadowCat::GetSkills() const
{
    return mSkillInputHandler->GetAssignedSkills();
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

    // Handle footstep sounds
    if (mIsMoving && !mIsDead)
    {
        mFootstepTimer -= deltaTime;
        
        if (mFootstepTimer <= 0.0f)
        {
            mFootstepTimer = 0.3f;  // Reset timer
            
            // Determine which sounds to use based on ground type
            std::string sound1, sound2;
            switch (mGame->GetGroundType())
            {
            case GroundType::Grass:
                sound1 = "e01_step_on_grass_small1.wav";
                sound2 = "e02_step_on_grass_small2.wav";
                break;
            case GroundType::Brick:
                sound1 = "e06_step_on_bricks1.wav";
                sound2 = "e07_step_on_bricks2.wav";
                break;
            case GroundType::Stone:
                sound1 = "e08_step_on_stone1.wav";
                sound2 = "e09_step_on_stone2.wav";
                break;
            }
            
            // Play random one of the two sounds
            mGame->GetAudio()->PlaySound(rand() % 2 ? sound1 : sound2, false, 0.6f);
        }
    }
    else
    {
        mFootstepTimer = 0.0f;  // Reset when not moving
    }
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

std::vector<UpgradeInfo> ShadowCat::GetRandomUpgrades() const
{
    std::vector<UpgradeInfo> allUpgrades;

    auto assignedSkills = mSkillInputHandler->GetAssignedSkills();
    for (auto skill : assignedSkills)
    {
        auto skillUpgrades = skill->GetAvailableUpgrades();
        allUpgrades.insert(allUpgrades.end(), skillUpgrades.begin(), skillUpgrades.end());
    }

    // Shuffle the upgrades with time
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(allUpgrades.begin(), allUpgrades.end(), g);

    std::vector<UpgradeInfo> selectedUpgrades;
    int upgradesToSelect = std::min(GameConstants::UPGRADE_COUNT, static_cast<int>(allUpgrades.size()));

    for (int i = 0; i < upgradesToSelect; ++i) selectedUpgrades.push_back(allUpgrades[i]);

    return selectedUpgrades;
}

void ShadowCat::ResetCollisionFilter() const
{
    CollisionFilter filter;
    filter.belongsTo = CollisionFilter::GroupMask({CollisionGroup::Player});
    filter.collidesWith = CollisionFilter::GroupMask({CollisionGroup::Environment, CollisionGroup::Enemy, CollisionGroup::EnemySkills});

    mColliderComponent->SetFilter(filter);
}