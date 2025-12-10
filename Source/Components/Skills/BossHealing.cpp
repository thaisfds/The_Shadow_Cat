#include "BossHealing.h"
#include "../../Actors/Characters/Character.h"
#include "../Drawing/AnimatorComponent.h"
#include "../../Game.h"
#include "../../SkillFactory.h"

BossHealing::BossHealing(Actor* owner, int updateOrder)
    : SkillBase(owner, updateOrder)
    , mHealedAt60(false)
    , mHealedAt40(false)
    , mHealedAt20(false)
{
    LoadSkillDataFromJSON("BossHealingData");

    float healDuration = mCharacter->GetComponent<AnimatorComponent>()->GetAnimationDuration("BossHealing");
    if (healDuration == 0.0f) healDuration = 1.0f;
    
    AddDelayedAction(0.0f, [this]() { Execute(); });
    AddDelayedAction(healDuration, [this]() { EndSkill(); });
}

nlohmann::json BossHealing::LoadSkillDataFromJSON(const std::string& fileName)
{
    auto data = SkillBase::LoadSkillDataFromJSON(fileName);

    // Get heal percentage from effects
    auto effects = data["effects"];
    for (const auto& effect : effects)
    {
        if (effect["type"] == "heal")
        {
            mHealPercent = effect["value"].get<float>();
            break;
        }
    }

    auto id = GameJsonParser::GetStringValue(data, "id");
    SkillFactory::Instance().RegisterSkill(id, [](Actor* owner) { return new BossHealing(owner); });

    return data;
}

void BossHealing::StartSkill(Vector2 targetPosition)
{
    SkillBase::StartSkill(targetPosition);

    // Try to play BossHealing animation if available
    // Note: The animation may need to be added to WhiteBossAnim.json or loaded separately
    auto animator = mCharacter->GetComponent<AnimatorComponent>();
    if (animator)
    {
        // Check if animation exists, if not, just play a default animation
        float duration = animator->GetAnimationDuration("BossHealing");
        if (duration > 0.0f)
        {
            animator->PlayAnimationOnce("BossHealing");
        }
        else
        {
            // Fallback to Idle or BasicAttack animation
            animator->PlayAnimationOnce("Idle");
        }
    }
    
    mCharacter->SetMovementLock(true);
    mCharacter->SetAnimationLock(true);
    
    // Play healing sound
    mCharacter->GetGame()->GetAudio()->PlaySound("e17_skill_acquired.mp3", false, 0.5f);
}

void BossHealing::Execute()
{
    // Heal 1/5 (20%) of max HP
    int maxHP = mCharacter->GetMaxHP();
    int healAmount = static_cast<int>(maxHP * mHealPercent);
    
    mCharacter->Heal(healAmount);
}

void BossHealing::EndSkill()
{
    SkillBase::EndSkill();
    
    mCharacter->SetMovementLock(false);
    mCharacter->SetAnimationLock(false);
}

bool BossHealing::ShouldTriggerHealing()
{
    if (mIsUsing || IsOnCooldown()) return false;

    int currentHP = mCharacter->GetHP();
    int maxHP = mCharacter->GetMaxHP();
    if (maxHP == 0) return false;
    
    float healthPercent = static_cast<float>(currentHP) / static_cast<float>(maxHP);

    // Check thresholds: 60%, 40%, 20%
    // Always trigger when reaching threshold (100% chance, but only once per threshold)
    if (healthPercent <= 0.6f && healthPercent > 0.4f && !mHealedAt60)
    {
        mHealedAt60 = true;
        return true;
    }
    else if (healthPercent <= 0.4f && healthPercent > 0.2f && !mHealedAt40)
    {
        mHealedAt40 = true;
        return true;
    }
    else if (healthPercent <= 0.2f && !mHealedAt20)
    {
        mHealedAt20 = true;
        return true;
    }

    return false;
}

