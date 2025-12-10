#pragma once

#include "../AIBehavior.h"

class ChaseBehavior : public AIBehavior
{
public:
    ChaseBehavior(Character* owner, float chaseRange = 250.0f);
    
    void OnEnter() override;
    void Update(float deltaTime) override;
    void OnExit() override;

    void LoadBehaviorData(const nlohmann::json& data) override;

    bool ChaseToPatrol() const;

private:
    Vector2 mLastKnownPlayerPos;
    float mChaseRange;
};
