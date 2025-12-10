#pragma once

#include "../AIBehavior.h"
#include <vector>
#include "../../Math.h"

class PatrolBehavior : public AIBehavior
{
public:
    PatrolBehavior(Character *owner, float patrolRadius = 100.0f);
    
    void OnEnter() override;
    void Update(float deltaTime) override;
    void OnExit() override;

    bool PatrolToChase();

private:
    float mPatrolRadius;
    Vector2 mPatrolCenter;
    Vector2 mCurrentWaypoint;
    Vector2 mPreviousPosition;
    float mPatrolPauseTimer;
    bool mDestinationReached;

    bool ShouldSelectNewWaypoint() const;
    void ResetPatrolPauseTimer();
};
