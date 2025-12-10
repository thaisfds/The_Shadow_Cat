#include "BasicAttack.h"

#include "../AnimatedParticleSystemComponent.h"
#include "../../Actors/Actor.h"
#include "../../Actors/Characters/Character.h"
#include "../Drawing/AnimatorComponent.h"
#include "../Drawing/DrawComponent.h"
#include "../Physics/Physics.h"
#include "../../Game.h"
#include "SkillBase.h"
#include "../../SkillFactory.h"
#include "../../Actors/Characters/ShadowCat.h"


BasicAttack::BasicAttack(Actor* owner, int updateOrder)
    : SkillBase(owner, updateOrder)
{
    LoadSkillDataFromJSON("BasicAttackData");

    float attackDuration = mCharacter->GetComponent<AnimatorComponent>()->GetAnimationDuration("BasicAttack");
    if (attackDuration == 0.0f) attackDuration = 1.0f;
    AddDelayedAction(0.53f, [this]() { Execute(); });
    AddDelayedAction(attackDuration, [this]() { EndSkill(); });
}

nlohmann::json BasicAttack::LoadSkillDataFromJSON(const std::string& fileName)
{
    auto data = SkillBase::LoadSkillDataFromJSON(fileName);

    mDamage = GameJsonParser::GetFloatEffectValue(data, "damage");
    mAreaOfEffect = GameJsonParser::GetAreaOfEffect(data);
    auto id = GameJsonParser::GetStringValue(data, "id");
    SkillFactory::Instance().RegisterSkill(id, [](Actor* owner) { return new BasicAttack(owner); });

    return data;
}

void BasicAttack::StartSkill(Vector2 targetPosition)
{
    SkillBase::StartSkill(targetPosition);
    mTargetVector -= mCharacter->GetPosition();
    mTargetVector.Normalize();

    mCharacter->GetComponent<AnimatorComponent>()->PlayAnimationOnce("BasicAttack");
    mCharacter->SetMovementLock(true);
    mCharacter->SetAnimationLock(true);  // Lock animations so attack anim isn't overridden
}

void BasicAttack::Execute()
{
    mCharacter->GetGame()->GetAttackTrailActor()->GetComponent<AnimatedParticleSystemComponent>()->EmitParticleAt(
        0.3f,
        0.0f,
        mCharacter->GetPosition() + mTargetVector * mRange,
        std::atan2(mTargetVector.y, mTargetVector.x),
        mCharacter->GetScale().x < 0.0f
    );

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

void BasicAttack::EndSkill()
{
    SkillBase::EndSkill();
    
    mCharacter->SetMovementLock(false);
    mCharacter->SetAnimationLock(false);  // Unlock animations
}

bool BasicAttack::EnemyShouldUse()
{
    auto player = mCharacter->GetGame()->GetPlayer();
    if (!player) return false;

    Vector2 toPlayer = player->GetPosition() - mCharacter->GetPosition();
    float distanceToPlayer = toPlayer.Length();

    return distanceToPlayer <= mRange;
}