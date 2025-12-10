#include "PatrolBehavior.h"
#include "../../Math.h"
#include "../../Random.h"
#include "../../Game.h"
#include "../../Actors/Characters/ShadowCat.h"

namespace
{
    constexpr float WAYPOINT_REACHED_DISTANCE = 10.0f;
    constexpr float MIN_POSITION_CHANGE_TO_DETECT_STUCK = 0.1f;
    constexpr float PATROL_PAUSE_MIN_DURATION = 1.0f;
    constexpr float PATROL_PAUSE_MAX_DURATION = 3.0f;

    constexpr float CHASE_DETECTION_RANGE = 100.0f;
    constexpr float VISION_DETECTION_RANGE = 250.0f;
    constexpr float DETECTION_ANGLE = Math::ToRadians(45.0f);
}

PatrolBehavior::PatrolBehavior(Character *owner, float patrolRadius)
    : AIBehavior(owner, "Patrol"), mPatrolRadius(patrolRadius)
{
    mPatrolCenter = mOwner->GetPosition();
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
        mCurrentWaypoint = Random::GetUnitVector() * Random::GetFloatRange(0.2f, mPatrolRadius) + mPatrolCenter;

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
    mPatrolPauseTimer = Random::GetFloatRange(PATROL_PAUSE_MIN_DURATION, PATROL_PAUSE_MAX_DURATION);
}

bool PatrolBehavior::PatrolToChase()
{
    const ShadowCat* player = mOwner->GetGame()->GetPlayer();
    if (!player) return false;

    Vector2 toPlayer = player->GetPosition() - mOwner->GetPosition();
    float distanceToPlayer = toPlayer.Length();

    if (distanceToPlayer > VISION_DETECTION_RANGE) return false;
    if (distanceToPlayer <= CHASE_DETECTION_RANGE) return true;

    toPlayer.Normalize();
    Vector2 forwardDir = mOwner->GetForward();

    float angleToPlayer = Math::Acos(Vector2::Dot(forwardDir, toPlayer));;
    return angleToPlayer <= DETECTION_ANGLE;
}