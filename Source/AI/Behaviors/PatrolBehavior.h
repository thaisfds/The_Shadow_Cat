#pragma once

#include "../AIBehavior.h"
#include <vector>
#include "../../Math.h"

class PatrolBehavior : public AIBehavior
{
public:
    PatrolBehavior(Character *owner, float patrolRadius = 100.0f, float patrolPauseMinDuration = 1.0f, float patrolPauseMaxDuration = 3.0f,
                   float chaseDetectionRange = 100.0f, float visionDetectionRange = 250.0f, float detectionAngleDegrees = 45.0f);
    
    void OnEnter() override;
    void Update(float deltaTime) override;
    void OnExit() override;

    void LoadBehaviorData(const nlohmann::json& data) override;

    bool PatrolToChase();

private:
    float mRadius;
    Vector2 mPatrolCenter;
    Vector2 mCurrentWaypoint;
    Vector2 mPreviousPosition;
    float mPatrolPauseTimer;
    bool mDestinationReached;

    float mPauseMinDuration;
    float mPauseMaxDuration;
    float mDetectionRange;
    float mVisionRange;
    float mDetectionAngle;

    bool ShouldSelectNewWaypoint() const;
    void ResetPatrolPauseTimer();
};
