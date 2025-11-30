#pragma once
#include "Character.h"

class BasicEnemy : public Character
{
public:
    BasicEnemy(class Game* game, float forwardSpeed = 0.0f, float patrolDistance = 200.0f);
    virtual ~BasicEnemy();

    void OnUpdate(float deltaTime) override;
    void TakeDamage(int damage) override;
    void Kill() override;

private:
    float mDeathTimer;
    bool mIsPlayingDeathAnim;
    
    // Patrol movement
    Vector2 mPatrolStartPos;
    float mPatrolDistance;
    int mPatrolDirection;  // 1 for right, -1 for left
    float mPatrolSpeed;
};
