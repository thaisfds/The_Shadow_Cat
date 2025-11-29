#pragma once
#include "Character.h"

class BasicEnemy : public Character
{
public:
    BasicEnemy(class Game* game, float forwardSpeed = 0.0f);
    virtual ~BasicEnemy();

    void OnUpdate(float deltaTime) override;
    void TakeDamage(int damage) override;
    void Kill() override;

private:
    float mDeathTimer;
    bool mIsPlayingDeathAnim;
};
