#include "WhiteBoss.h"
#include "../../../GameConstants.h"
#include "../../../AI/AIStateMachine.h"
#include "../../../AI/Behaviors/PatrolBehavior.h"
#include "../../../AI/Behaviors/ChaseBehavior.h"
#include "../../../AI/Behaviors/SkillBehavior.h"
#include "../../../Components/Drawing/AnimatorComponent.h"
#include "../../../Random.h"

WhiteBoss::WhiteBoss(class Game* game, Vector2 position, float forwardSpeed)
	: BossBase(game, position, forwardSpeed)
{
	mAnimatorComponent = new AnimatorComponent(this, "WhiteBossAnim", GameConstants::TILE_SIZE * 2.0f, GameConstants::TILE_SIZE * 2.0f);

	auto data = LoadEnemyDataFromJSON("WhiteBossData");
	SetupAIBehaviors(data);
}

void WhiteBoss::SetupAIBehaviors(const nlohmann::json& data)
{
	mStateMachine = new AIStateMachine(this);

	auto patrol = new PatrolBehavior(this);
	auto chase = new ChaseBehavior(this);
	auto skill = new SkillBehavior(this);

	patrol->LoadBehaviorData(data);
	chase->LoadBehaviorData(data);

	mStateMachine->RegisterState(patrol);
	mStateMachine->RegisterState(chase);
	mStateMachine->RegisterState(skill);

	skill->AddTransition(patrol->GetName(), [skill]() { return skill->SkillToPatrol(); });
	skill->AddTransitionCondition([skill]() 
	{
		if (Random::GetFloatRange(0.0f, 1.0f) > 0.5f) return false;
		return skill->AnySkillAvailable();
	});

	mStateMachine->SetInitialState(patrol->GetName());
}