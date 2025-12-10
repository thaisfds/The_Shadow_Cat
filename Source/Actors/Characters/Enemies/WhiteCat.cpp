#include "WhiteCat.h"
#include "../../../GameConstants.h"
#include "../../../AI/AIStateMachine.h"
#include "../../../Components/Drawing/AnimatorComponent.h"
#include "../../../AI/Behaviors/PatrolBehavior.h"
#include "../../../AI/Behaviors/ChaseBehavior.h"
#include "../../../AI/Behaviors/SkillBehavior.h"

WhiteCat::WhiteCat(class Game* game, Vector2 position, float forwardSpeed)
	: EnemyBase(game, position, forwardSpeed)
{
	mAnimatorComponent = new AnimatorComponent(this, "WhiteCatAnim", GameConstants::TILE_SIZE, GameConstants::TILE_SIZE);

	auto data = LoadEnemyDataFromJSON("WhiteCatData");
	SetupAIBehaviors(data);
}

void WhiteCat::SetupAIBehaviors(const nlohmann::json& data)
{
	mStateMachine = new AIStateMachine(this);

	auto patrol = new PatrolBehavior(this);
	auto chase = new ChaseBehavior(this);
	auto skill = new SkillBehavior(this);
    
	patrol->LoadBehaviorData(data);
	chase->LoadBehaviorData(data);
	skill->LoadBehaviorData(data);
	
	mStateMachine->RegisterState(patrol);
	mStateMachine->RegisterState(chase);
	mStateMachine->RegisterState(skill);

	patrol->AddTransition(chase->GetName(), [patrol]() { return patrol->PatrolToChase(); });
	chase->AddTransition(patrol->GetName(), [chase]() { return chase->ChaseToPatrol(); });
	skill->AddTransition(patrol->GetName(), [skill]() { return skill->SkillToPatrol(); });
	skill->AddTransitionCondition([skill]() { return skill->AnySkillAvailable(); });
	
	mStateMachine->SetInitialState(patrol->GetName());

}