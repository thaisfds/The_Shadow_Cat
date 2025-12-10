#include "SkillBehavior.h"
#include "../../Game.h"
#include "../../Actors/Characters/ShadowCat.h"

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
	for (auto& skill : mOwner->GetSkills())
	{
		if (!skill) continue; // Skip null skills
		if (skill->CanUse(playerPos) && skill->EnemyShouldUse())
		{
			skill->StartSkill(playerPos);
			break;
		}
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