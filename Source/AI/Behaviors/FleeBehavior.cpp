#include "FleeBehavior.h"

#include "../../Actors/Characters/ShadowCat.h"
#include "../../Actors/Characters/Enemies/OrangeCat.h"

FleeBehavior::FleeBehavior(Character* owner, SkillBase* fleeSkill, float fleeDistance)
	: AIBehavior(owner, "Flee"), mFleeDistance(fleeDistance), mFleeSkill(fleeSkill)
{
}

void FleeBehavior::LoadBehaviorData(const nlohmann::json& data)
{
	mFleeDistance = GameJsonParser::GetValue<float>(data, "aiBehaviors.flee.distance", mFleeDistance);
	mSpeedMultiplier = GameJsonParser::GetValue<float>(data, "aiBehaviors.flee.speedMultiplier", mSpeedMultiplier);
	mFleeCooldown = GameJsonParser::GetValue<float>(data, "aiBehaviors.flee.cooldown", mFleeCooldown);
}

void FleeBehavior::OnEnter()
{
	mLastPosition = Vector2::Zero;
	mOwner->SetSpeedMultiplier(mSpeedMultiplier);
}

void FleeBehavior::Update(float deltaTime)
{
	const ShadowCat* player = mOwner->GetGame()->GetPlayer();
	if (!player) return;
	if (mOwner->IsUsingSkill()) return;

	mOwner->StopMovement();

	Vector2 playerPos = player->GetPosition();
	Vector2 toPlayer = playerPos - mOwner->GetPosition();
	float distanceToPlayer = toPlayer.Length();
	if (distanceToPlayer >= mFleeDistance)
	{
		mLeaveState = true;
		return;
	}

	toPlayer.Normalize();
	Vector2 fleeTarget = playerPos - toPlayer * mFleeDistance;

	if (mFleeSkill && mFleeSkill->CanUse(fleeTarget))
		mFleeSkill->StartSkill(fleeTarget);

	mOwner->MoveToward(fleeTarget);
	SDL_Log("Fleeing to (%.2f, %.2f)", fleeTarget.x, fleeTarget.y);
	SDL_Log("Player at (%.2f, %.2f)", playerPos.x, playerPos.y);
	SDL_Log("Distance to player: %.2f", distanceToPlayer);

	mLeaveState = Math::NearlyZero(Vector2::Distance(mOwner->GetPosition(), mLastPosition));

	mLastPosition = mOwner->GetPosition();
}

void FleeBehavior::OnExit()
{
	mOwner->StopMovement();
	mOwner->SetSpeedMultiplier(1.0f);
	((OrangeCat*)mOwner)->SetFleeTimer(mFleeCooldown);
}