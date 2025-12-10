#include "WhiteSlash.h"

#include "../AnimatedParticleSystemComponent.h"
#include "../../Actors/Actor.h"
#include "../../Actors/Characters/Character.h"
#include "../Drawing/AnimatorComponent.h"
#include "../Physics/Physics.h"
#include "../../Game.h"
#include "../../SkillFactory.h"
#include "../../Actors/Characters/ShadowCat.h"

WhiteSlash::WhiteSlash(Actor* owner, int updateOrder)
    : SkillBase(owner, updateOrder)
{
    LoadSkillDataFromJSON("WhiteSlashData");

    float attackDuration = mCharacter->GetComponent<AnimatorComponent>()->GetAnimationDuration("WhiteSlash");
    if (attackDuration == 0.0f) attackDuration = 0.5f;
    AddDelayedAction(0.3f, [this]() { Execute(); });
    AddDelayedAction(attackDuration, [this]() { EndSkill(); });
}

nlohmann::json WhiteSlash::LoadSkillDataFromJSON(const std::string& fileName)
{
    auto data = SkillBase::LoadSkillDataFromJSON(fileName);

    mDamage = GameJsonParser::GetFloatEffectValue(data, "damage");
    mAreaOfEffect = GameJsonParser::GetAreaOfEffect(data);
    auto id = GameJsonParser::GetStringValue(data, "id");
    SkillFactory::Instance().RegisterSkill(id, [](Actor* owner) { return new WhiteSlash(owner); });

    return data;
}

void WhiteSlash::StartSkill(Vector2 targetPosition)
{
    SkillBase::StartSkill(targetPosition);
    mTargetVector -= mCharacter->GetPosition();
    mTargetVector.Normalize();

    mCharacter->GetComponent<AnimatorComponent>()->PlayAnimationOnce("WhiteSlash");
    mCharacter->SetMovementLock(true);
    mCharacter->SetAnimationLock(true);
    
    // Play boss attack sound
    int choice = rand() % 3;
    std::string sound = choice == 0 ? "s08_boss_simple_attack1.wav" : 
                        choice == 1 ? "s09_boss_simple_attack2.wav" : "s10_boss_simple_attack3.wav";
    mCharacter->GetGame()->GetAudio()->PlaySound(sound, false, 0.5f);
}

void WhiteSlash::Execute()
{
    // Emit WhiteSlash particle effect (similar to AttackTrail)
    mCharacter->GetGame()->GetWhiteSlashActor()->GetComponent<AnimatedParticleSystemComponent>()->EmitParticleAt(
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
        if (enemyCharacter)
        {
            enemyCharacter->TakeDamage(mDamage);
        }
    }

    if (mCharacter->GetGame()->IsDebugging())
    {
        auto vertices = ((PolygonCollider*)mAreaOfEffect)->GetVertices();
        for (auto& v : vertices) v += pos;
        Physics::DebugDrawPolygon(mCharacter->GetGame(), vertices, 0.5f, 15);
    }
}

void WhiteSlash::EndSkill()
{
    SkillBase::EndSkill();
    
    mCharacter->SetMovementLock(false);
    mCharacter->SetAnimationLock(false);
}

bool WhiteSlash::EnemyShouldUse()
{
    auto player = mCharacter->GetGame()->GetPlayer();
    if (!player) return false;

    Vector2 toPlayer = player->GetPosition() - mCharacter->GetPosition();
    float distanceToPlayer = toPlayer.Length();

    return distanceToPlayer <= mRange;
}

