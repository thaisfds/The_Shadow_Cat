#include "WhiteBubble.h"
#include "../../Game.h"
#include "../../GameConstants.h"
#include "../Drawing/AnimatorComponent.h"
#include "../../Actors/Characters/Character.h"
#include "../Physics/ColliderComponent.h"
#include "../Physics/Physics.h"
#include "../../SkillFactory.h"
#include "../../Actors/Characters/ShadowCat.h"

WhiteBubble::WhiteBubble(Actor* owner, int updateOrder)
	: SkillBase(owner, updateOrder)
	, mDuration(20.0f)
{
	LoadSkillDataFromJSON("WhiteBubbleData");

	float bubbleDuration = mCharacter->GetComponent<AnimatorComponent>()->GetAnimationDuration("WhiteBubble");
	if (bubbleDuration == 0.0f) bubbleDuration = 0.5f;
	
	AddDelayedAction(0.5f, [this]() { Execute(); });
	AddDelayedAction(bubbleDuration, [this]() { EndSkill(); });
}

nlohmann::json WhiteBubble::LoadSkillDataFromJSON(const std::string& fileName)
{
	auto data = SkillBase::LoadSkillDataFromJSON(fileName);

	mDamage = static_cast<int>(GameJsonParser::GetFloatEffectValue(data, "damage"));
	mDuration = GameJsonParser::GetValue<float>(data, "duration", 20.0f);
	mAreaOfEffect = GameJsonParser::GetAreaOfEffect(data);
	auto id = GameJsonParser::GetStringValue(data, "id");
	SkillFactory::Instance().RegisterSkill(id, [](Actor* owner) { return new WhiteBubble(owner); });

	return data;
}

void WhiteBubble::Execute()
{
	// Play bubble sound
	mCharacter->GetGame()->GetAudio()->PlaySound("s05_furball_launch1.wav", false, 0.5f);

	// Spawn bubble at target position (where boss is aiming)
	// mTargetVector contains the target position after StartSkill
	mCharacter->GetGame()->GetWhiteBubbleActor()->Awake(
		mTargetVector, // Position where bubble spawns (target position from StartSkill)
		mDamage,
		mCharacter->GetSkillFilter(),
		mAreaOfEffect,
		mDuration
	);
}

void WhiteBubble::StartSkill(Vector2 targetPosition)
{
	SkillBase::StartSkill(targetPosition);
	// mTargetVector now contains the target position (not normalized)

	mCharacter->GetComponent<AnimatorComponent>()->PlayAnimationOnce("WhiteBomb"); // Use WhiteBomb animation for now
	mCharacter->SetMovementLock(true);
}

void WhiteBubble::EndSkill()
{
	SkillBase::EndSkill();

	mCharacter->SetMovementLock(false);
}

bool WhiteBubble::EnemyShouldUse()
{
	auto player = mCharacter->GetGame()->GetPlayer();
	if (!player) return false;

	Vector2 toPlayer = player->GetPosition() - mCharacter->GetPosition();
	float distanceToPlayer = toPlayer.Length();

	return distanceToPlayer <= mRange;
}

WhiteBubbleActor::WhiteBubbleActor(class Game* game)
	: Actor(game)
	, mDamageCooldown(0.5f) // Damage every 0.5 seconds
	, mLastDamageTime(0.0f)
{
	mAnimatorComponent = new AnimatorComponent(this, "WhiteBubbleAnim", GameConstants::TILE_SIZE * 2.0f, GameConstants::TILE_SIZE * 2.0f);
	CollisionFilter filter;
	mColliderComponent = new ColliderComponent(this, 0, 0, nullptr, filter);
	Kill();
}

WhiteBubbleActor::~WhiteBubbleActor()
{
}

void WhiteBubbleActor::OnUpdate(float deltaTime)
{
	if (mDead) return;

	mDuration -= deltaTime;
	if (mDuration <= 0.0f)
	{
		Kill();
		return;
	}

	// Check for player collision and deal damage periodically
	mLastDamageTime += deltaTime;
	if (mLastDamageTime >= mDamageCooldown)
	{
		ColliderComponent* colliderComp = GetComponent<ColliderComponent>();
		if (colliderComp && colliderComp->GetCollider())
		{
			auto hitColliders = Physics::GetOverlappingColliders(GetGame(), colliderComp->GetCollider());
			for (auto collider : hitColliders)
			{
				auto actor = collider->GetOwner();
				auto playerCharacter = dynamic_cast<ShadowCat*>(actor);
				if (playerCharacter && !playerCharacter->IsDead())
				{
					playerCharacter->TakeDamage(mDamage);
					mLastDamageTime = 0.0f; // Reset cooldown after dealing damage
					break; // Only damage once per check
				}
			}
		}
	}

	mDelayedActions.Update(deltaTime);
}

void WhiteBubbleActor::Kill()
{
	mAnimatorComponent->SetVisible(false);
	mAnimatorComponent->SetEnabled(false);
	mColliderComponent->SetEnabled(false);
	mColliderComponent->SetCollider(nullptr);
	mColliderComponent->SetDebugDrawIfDisabled(false);
	mDead = true;

	mDelayedActions.Clear();
}

void WhiteBubbleActor::Awake(Vector2 position, int damage, CollisionFilter filter, Collider* areaOfEffect, float duration)
{
	mAnimatorComponent->SetEnabled(true);
	mAnimatorComponent->SetVisible(true);
	mColliderComponent->SetEnabled(true);
	mColliderComponent->SetFilter(filter);
	mColliderComponent->SetCollider(areaOfEffect);
	mColliderComponent->SetDebugDrawIfDisabled(true);

	// Set size based on collider radius
	int radius = ((CircleCollider*)areaOfEffect)->GetRadius();
	int dim = radius * 2;
	mAnimatorComponent->SetSize(Vector2(dim, dim));
	
	SetPosition(position);
	mDamage = damage;
	mDuration = duration;
	mLastDamageTime = 0.0f;
	mDead = false;
	
	mDelayedActions.Reset();
}

