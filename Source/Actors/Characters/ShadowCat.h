//
// Created by thais on 11/5/2025.
//

#pragma once
#include "Character.h"

class ShadowCat : public Character
{
public:
    explicit ShadowCat(Game *game, float forwardSpeed = 200.0f);

    void OnProcessInput(const Uint8 *keyState) override;
    void OnHandleEvent(const SDL_Event& event) override;
    void OnUpdate(float deltaTime) override;

    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent *other) override;
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent *other) override;

    void Kill() override;

private:
    class ParticleSystemComponent *mFireBalls;
    class SkillInputHandler *mSkillInputHandler;

    float mFireBallCooldown = 1.0f;
};