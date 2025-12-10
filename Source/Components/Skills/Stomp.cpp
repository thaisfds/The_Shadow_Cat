#include "Stomp.h"

#include "../Drawing/AnimatorComponent.h"
#include "../Physics/CollisionFilter.h"
#include "../Physics/Physics.h"
#include "../../Actors/Characters/Character.h"
#include "../../Game.h"
#include "../../GameConstants.h"
#include "../Physics/ColliderComponent.h"
#include "SkillBase.h"
#include "../../SkillFactory.h"


Stomp::Stomp(Actor* owner, int updateOrder)
	: SkillBase(owner, updateOrder)
{
	LoadSkillDataFromJSON("StompData");
}

nlohmann::json Stomp::LoadSkillDataFromJSON(const std::string& fileName)
{
	auto data = SkillBase::LoadSkillDataFromJSON(fileName);

	mDamage = GameJsonParser::GetFloatEffectValue(data, "damage");
	mAreaOfEffect = GameJsonParser::GetAreaOfEffect(data);
	mRadius = mAreaOfEffect ? ((CircleCollider*)mAreaOfEffect)->GetRadius() : 0.0f;
	auto id = GameJsonParser::GetStringValue(data, "id");
	SkillFactory::Instance().RegisterSkill(id, [](Actor* owner) { return new Stomp(owner); });

	mUpgrades.push_back(GameJsonParser::GetUpgradeInfo(this, data, "damage", &mDamage));
	mUpgrades.push_back(GameJsonParser::GetUpgradeInfo(this, data, "cooldown", &mCooldown));
	mUpgrades.push_back(GameJsonParser::GetUpgradeInfo(this, data, "range", &mRange));
	mUpgrades.push_back(GameJsonParser::GetUpgradeInfo(this, data, "radius", &mRadius));

	return data;
}

void Stomp::StartSkill(Vector2 targetPosition)
{
	mCurrentCooldown = mCooldown;

	((CircleCollider*)mAreaOfEffect)->SetRadius(mRadius);

	// Play stomp sound
	std::string sound = rand() % 2 ? "s03_stomp_attack1.wav" : "s04_stomp_attack2.wav";
	mCharacter->GetGame()->GetAudio()->PlaySound(sound, false, 0.7f);

	mCharacter->GetGame()->GetStompActor()->Awake(
		targetPosition,
		mDamage,
		0.65f,
		mCharacter->GetSkillFilter(),
		mAreaOfEffect
	);
}

StompActor::StompActor(class Game* game)
	: Actor(game)
	, mDamage(0)
{
	mAnimatorComponent = new AnimatorComponent(this, "StompAnim", GameConstants::TILE_SIZE, GameConstants::TILE_SIZE);
	CollisionFilter filter;
	mColliderComponent = new ColliderComponent(this, 0, 0, nullptr, filter);
	Kill();
}

StompActor::~StompActor()
{
}

bool Stomp::CanUse(Vector2 targetPosition, bool showRangeOnFalse) const
{
	if (!SkillBase::CanUse(targetPosition, showRangeOnFalse)) return false;
	
	Vector2 position = mCharacter->GetPosition();
	bool inRange = (targetPosition - position).Length() <= mRange;
	if (!inRange && showRangeOnFalse) mDrawRangeTimer = GameConstants::DRAW_SKILL_RANGE_DURATION;

	return inRange;
}

void StompActor::OnUpdate(float deltaTime)
{
	mDelayedActions.Update(deltaTime);
}

void StompActor::Execute()
{
	ColliderComponent* colliderComp = GetComponent<ColliderComponent>();
	Vector2 position = GetPosition();

	auto hitColliders = Physics::GetOverlappingColliders(GetGame(), colliderComp->GetCollider());
	for (auto collider : hitColliders)
	{
		auto enemyActor = collider->GetOwner();
		auto enemyCharacter = dynamic_cast<Character*>(enemyActor);
		enemyCharacter->TakeDamage(mDamage);
	}
}

void StompActor::Kill()
{
	mAnimatorComponent->SetVisible(false);
	mAnimatorComponent->SetEnabled(false);
	mColliderComponent->SetEnabled(false);
	mColliderComponent->SetDebugDrawIfDisabled(false);
	mDead = true;

	mDelayedActions.Clear();
}

void StompActor::Awake(Vector2 position, int damage, float delay, CollisionFilter filter, Collider *areaOfEffect)
{
	mAnimatorComponent->SetEnabled(true);
	mAnimatorComponent->SetVisible(true);
	mAnimatorComponent->PlayAnimationOnce("Stomp");
	mColliderComponent->SetFilter(filter);
	mColliderComponent->SetCollider(areaOfEffect);
	mColliderComponent->SetDebugDrawIfDisabled(true);

	int dim = ((CircleCollider*)areaOfEffect)->GetRadius() * 2;
	mAnimatorComponent->SetSize(Vector2(dim, dim));
	
	mDamage = damage;
	mDead = false;
	SetPosition(position);

	mDelayedActions.Reset();
	
	float stompLifetime = GetComponent<AnimatorComponent>()->GetAnimationDuration("Stomp");
	AddDelayedAction(delay, [this]() { Execute(); });
	AddDelayedAction(stompLifetime, [this]() { Kill(); });
}