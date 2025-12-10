#include "PatrolBehavior.h"
#include "../../Math.h"
#include "../../Random.h"
#include "../../Game.h"
#include "../../Actors/Characters/ShadowCat.h"

namespace
{
    constexpr float WAYPOINT_REACHED_DISTANCE = 10.0f;
    constexpr float MIN_POSITION_CHANGE_TO_DETECT_STUCK = 0.1f;
}

PatrolBehavior::PatrolBehavior(Character *owner, float patrolRadius, float patrolPauseMinDuration, float patrolPauseMaxDuration,
                               float chaseDetectionRange, float visionDetectionRange, float detectionAngleDegrees)
    : AIBehavior(owner, "Patrol"), mRadius(patrolRadius), mPauseMinDuration(patrolPauseMinDuration), mPauseMaxDuration(patrolPauseMaxDuration),
      mDetectionRange(chaseDetectionRange), mVisionRange(visionDetectionRange), mDetectionAngle(Math::ToRadians(detectionAngleDegrees))
{
    mPatrolCenter = mOwner->GetPosition();
}

void PatrolBehavior::LoadBehaviorData(const nlohmann::json& data)
{
    mRadius = GameJsonParser::GetValue<float>(data, "aiBehaviors.patrol.radius", mRadius);
    mPauseMinDuration = GameJsonParser::GetValue<float>(data, "aiBehaviors.patrol.pauseMinDuration", mPauseMinDuration);
    mPauseMaxDuration = GameJsonParser::GetValue<float>(data, "aiBehaviors.patrol.pauseMaxDuration", mPauseMaxDuration);
    mDetectionRange = GameJsonParser::GetValue<float>(data, "aiBehaviors.patrol.detectionRange", mDetectionRange);
    mVisionRange = GameJsonParser::GetValue<float>(data, "aiBehaviors.patrol.visionRange", mVisionRange);
    float detectionAngleDegrees = GameJsonParser::GetValue<float>(data, "aiBehaviors.patrol.detectionAngleDegrees", Math::ToDegrees(mDetectionAngle));
    mDetectionAngle = detectionAngleDegrees > 0.0f ? Math::ToRadians(detectionAngleDegrees) : mDetectionAngle;
}

void PatrolBehavior::OnEnter()
{
    mCurrentWaypoint = Vector2::Zero;
    mDestinationReached = true;
    mOwner->StopMovement();
    mPreviousPosition = mOwner->GetPosition();
    ResetPatrolPauseTimer();
}

void PatrolBehavior::Update(float deltaTime)
{
    if (!mOwner) return;

    mOwner->StopMovement();

    mPatrolPauseTimer -= deltaTime;
    if (mPatrolPauseTimer > 0.0f) return;

    if (ShouldSelectNewWaypoint())
        mCurrentWaypoint = Random::GetUnitVector() * Random::GetFloatRange(0.2f, mRadius) + mPatrolCenter;

    mOwner->MoveToward(mCurrentWaypoint);
    mPreviousPosition = mOwner->GetPosition();

    mDestinationReached = Vector2::Distance(mOwner->GetPosition(), mCurrentWaypoint) <= WAYPOINT_REACHED_DISTANCE;
    if (mDestinationReached) ResetPatrolPauseTimer();
}

bool PatrolBehavior::ShouldSelectNewWaypoint() const
{
    if (Math::NearlyEqual(mCurrentWaypoint, Vector2::Zero)) return true;
    if (mDestinationReached) return true;
    if (Vector2::Distance(mOwner->GetPosition(), mPreviousPosition) < MIN_POSITION_CHANGE_TO_DETECT_STUCK) return true;

    return false;
}

void PatrolBehavior::OnExit()
{
}

void PatrolBehavior::ResetPatrolPauseTimer()
{
    mPatrolPauseTimer = Random::GetFloatRange(mPauseMinDuration, mPauseMaxDuration);
}

bool PatrolBehavior::PatrolToChase()
{
    const ShadowCat* player = mOwner->GetGame()->GetPlayer();
    if (!player) return false;

    Vector2 toPlayer = player->GetPosition() - mOwner->GetPosition();
    float distanceToPlayer = toPlayer.Length();

    if (distanceToPlayer <= mDetectionRange) return true;
    if (distanceToPlayer > mVisionRange) return false;

    toPlayer.Normalize();
    Vector2 forwardDir = mOwner->GetForward();

    float angleToPlayer = Math::Acos(Vector2::Dot(forwardDir, toPlayer));;
    return angleToPlayer <= mDetectionAngle;
}