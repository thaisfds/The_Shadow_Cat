//
// Created by thais on 11/5/2025.
//

#pragma once
#include "Actor.h"

class ShadowCat : public Actor
{
public:
    explicit ShadowCat(Game *game, float forwardSpeed = 300.0f, float jumpSpeed = -750.0f);

    void OnProcessInput(const Uint8 *keyState) override;
    void OnUpdate(float deltaTime) override;

    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent *other) override;
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent *other) override;

    void Kill() override;

private:
    void ManageAnimations();

    float mForwardSpeed;
    float mJumpSpeed;
    bool mIsRunning;
    bool mIsDead;

    class RigidBodyComponent *mRigidBodyComponent;
    class AnimatorComponent *mDrawComponent;
    class AABBColliderComponent *mColliderComponent;
    class ParticleSystemComponent *mFireBalls;

    float mFireBallCooldown = 1.0f;
};