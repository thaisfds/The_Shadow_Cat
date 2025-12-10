#include "ChaseBehavior.h"
#include "../../Actors/Characters/ShadowCat.h"

namespace
{
    constexpr float LAST_KNOWN_POS_THRESHOLD = 20.0f;
}

ChaseBehavior::ChaseBehavior(Character* owner, float chaseRange) : AIBehavior(owner, "Chase"), mChaseRange(chaseRange) {}

void ChaseBehavior::LoadBehaviorData(const nlohmann::json& data)
{
    mChaseRange = GameJsonParser::GetValue<float>(data, "aiBehaviors.chase.range", mChaseRange);
}

void ChaseBehavior::OnEnter()
{
    mOwner->StopMovement();
}

void ChaseBehavior::Update(float deltaTime)
{
    const ShadowCat* player = mOwner->GetGame()->GetPlayer();
    if (!player) return;
    
    mOwner->StopMovement();
    
    float distanceToPlayer = (player->GetPosition() - mOwner->GetPosition()).Length();
    auto pos = distanceToPlayer <= mChaseRange ? player->GetPosition() : mLastKnownPlayerPos;
    mLastKnownPlayerPos = pos;
    
    mOwner->MoveToward(pos);
}

void ChaseBehavior::OnExit()
{
    mOwner->StopMovement();
}

bool ChaseBehavior::ChaseToPatrol() const
{
    const ShadowCat* player = mOwner->GetGame()->GetPlayer();
    if (!player) return true;

    Vector2 toLastKnown = mLastKnownPlayerPos - mOwner->GetPosition();
    float distanceToLastKnown = toLastKnown.Length();

    return distanceToLastKnown < LAST_KNOWN_POS_THRESHOLD;
}
