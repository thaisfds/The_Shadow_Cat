#include "ClawAttack.h"
#include "../../Game.h"
#include "../../Actors/Characters/Character.h"
#include "../Drawing/AnimatorComponent.h"
#include "../Physics/Physics.h"
#include "../Physics/ColliderComponent.h"
#include "../../SkillFactory.h"

ClawAttack::ClawAttack(Actor* owner, int updateOrder)
	: SkillBase(owner, updateOrder)
{
	LoadSkillDataFromJSON("ClawAttackData");

	float attackDuration = mCharacter->GetComponent<AnimatorComponent>()->GetAnimationDuration("ClawAttack");
	if (attackDuration == 0.0f) attackDuration = 1.0f;

	float jumpStartTime = 1.25f;
	float jumpEndTime = 1.35f;
	float backwardsJumpDelay = 0.2f;

	float forwardDistance = mForwardSpeed * (jumpEndTime - jumpStartTime);
	float backwardDistance = -forwardDistance * mBackwardDistancePercentage;
	mBackwardSpeed = backwardDistance / (attackDuration - (jumpEndTime + backwardsJumpDelay));

	AddDelayedAction(jumpStartTime, [this]() { mVelocity = mTargetVector * mForwardSpeed;});
	AddDelayedAction(jumpEndTime, [this]() { Execute(); });
	AddDelayedAction(jumpEndTime + backwardsJumpDelay, [this]() { mVelocity = mTargetVector * mBackwardSpeed; });
	AddDelayedAction(attackDuration, [this]() { EndSkill(); });
}

nlohmann::json ClawAttack::LoadSkillDataFromJSON(const std::string& fileName)
{
	auto data = SkillBase::LoadSkillDataFromJSON(fileName);

	mDamage = GameJsonParser::GetFloatEffectValue(data, "damage");
	mForwardSpeed = GameJsonParser::GetFloatEffectValue(data, "forwardSpeed");
	mBackwardDistancePercentage = GameJsonParser::GetFloatEffectValue(data, "backwardDistancePercentage");
	mAreaOfEffect = GameJsonParser::GetAreaOfEffect(data);
	auto id = GameJsonParser::GetStringValue(data, "id");

	return data;
}

void ClawAttack::Update(float deltaTime)
{
	SkillBase::Update(deltaTime);

	if (!mIsUsing) return;

	mCharacter->GetComponent<RigidBodyComponent>()->SetVelocity(mVelocity);
}

void ClawAttack::Execute()
{
	mVelocity = Vector2::Zero;

	auto collisionActor = mCharacter->GetGame()->GetCollisionQueryActor();

	((PolygonCollider*)mAreaOfEffect)->SetForward(mTargetVector);
	collisionActor->GetComponent<ColliderComponent>()->SetCollider(mAreaOfEffect);
	collisionActor->GetComponent<ColliderComponent>()->SetFilter(mCharacter->GetSkillFilter());

	auto pos = mCharacter->GetPosition();
	auto hitColliders = mAreaOfEffect->GetOverlappingCollidersAt(&pos);
	for (auto collider : hitColliders)
	{
		auto enemyActor = collider->GetOwner();
		auto enemyCharacter = dynamic_cast<Character*>(enemyActor);
		enemyCharacter->TakeDamage(mDamage);
	}

	if (mCharacter->GetGame()->IsDebugging())
	{
		auto vertices = ((PolygonCollider*)mAreaOfEffect)->GetVertices();
		for (auto& v : vertices) v += pos;
		Physics::DebugDrawPolygon(mCharacter->GetGame(), vertices, 0.5f, 15);
	}

}

void ClawAttack::StartSkill(Vector2 targetPosition)
{
	SkillBase::StartSkill(targetPosition);
	mTargetVector -= mCharacter->GetPosition();
	mTargetVector.Normalize();

	mCharacter->GetComponent<AnimatorComponent>()->PlayAnimationOnce("ClawAttack");
	mCharacter->SetMovementLock(true);
	
	// Play claw attack sound
	std::string sound = rand() % 2 ? "s11_claw_attack1.wav" : "s12_claw_attack2.wav";
	mCharacter->GetGame()->GetAudio()->PlaySound(sound, false, 0.7f);
}

void ClawAttack::EndSkill()
{
	SkillBase::EndSkill();

	mVelocity = Vector2::Zero;
	mCharacter->SetMovementLock(false);
}