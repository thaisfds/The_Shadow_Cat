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
	auto player = mOwner->GetGame()->GetPlayer();
	if (!player) return;

	auto playerPos = player->GetPosition();
	
	// Collect all available skills
	std::vector<SkillBase*> availableSkills;
	for (auto& skill : mOwner->GetSkills())
	{
		if (!skill) continue; // Skip null skills
		if (skill->CanUse(playerPos) && skill->EnemyShouldUse())
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
	for (auto& skill : mOwner->GetSkills())
	{
		if (!skill) continue; // Skip null skills
		if (skill->EnemyShouldUse()) return true;
	}

	return false;
}