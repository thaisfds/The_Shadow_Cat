#pragma once

#include "../AIBehavior.h"

class ChaseBehavior : public AIBehavior
{
public:
    ChaseBehavior(Character* owner);
    
    void OnEnter() override;
    void Update(float deltaTime) override;
    void OnExit() override;

    bool ChaseToPatrol() const;

private:
    Vector2 mLastKnownPlayerPos;
};
