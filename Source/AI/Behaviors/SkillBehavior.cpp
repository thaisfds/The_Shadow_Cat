#include "SkillBehavior.h"
#include "../../Game.h"
#include "../../Actors/Characters/ShadowCat.h"
#include "../../Random.h"
#include <vector>

SkillBehavior::SkillBehavior(Character* owner)
	: AIBehavior(owner, "Skill")
{
}

void SkillBehavior::OnEnter()
{
}

void SkillBehavior::Update(float deltaTime)
{
	// Don't use skills if already using one
	if (mOwner->IsUsingSkill()) return;
	
	auto player = mOwner->GetGame()->GetPlayer();
	if (!player) return;

	auto playerPos = player->GetPosition();
	
	// Collect all available skills
	std::vector<SkillBase*> availableSkills;
	for (auto& skill : mOwner->GetSkills())
	{
		if (!skill) continue; // Skip null skills
		if (skill->CanUse(playerPos) && skill->EnemyShouldUse() && !skill->IsOnCooldown())
		{
			availableSkills.push_back(skill);
		}
	}
	
	// If we have available skills, choose one randomly
	if (!availableSkills.empty())
	{
		int randomIndex = Random::GetIntRange(0, static_cast<int>(availableSkills.size()) - 1);
		availableSkills[randomIndex]->StartSkill(playerPos);
	}
}

void SkillBehavior::OnExit()
{
}

bool SkillBehavior::SkillToPatrol()
{
	return !mOwner->IsUsingSkill();
}

bool SkillBehavior::AnySkillAvailable() const
{
	auto player = mOwner->GetGame()->GetPlayer();
	if (!player) return false;
	
	auto playerPos = player->GetPosition();
	
	for (auto& skill : mOwner->GetSkills())
	{
		if (!skill) continue; // Skip null skills
		// Check if skill can be used (not on cooldown, in range, and should be used)
		if (skill->EnemyShouldUse() && !skill->IsOnCooldown() && skill->CanUse(playerPos))
		{
			return true;
		}
	}

	return false;
}