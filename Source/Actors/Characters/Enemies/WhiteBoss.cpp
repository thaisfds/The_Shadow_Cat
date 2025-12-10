#include "WhiteBoss.h"
#include "../../../GameConstants.h"
#include "../../../AI/AIStateMachine.h"
#include "../../../AI/Behaviors/PatrolBehavior.h"
#include "../../../AI/Behaviors/ChaseBehavior.h"
#include "../../../AI/Behaviors/SkillBehavior.h"
#include "../../../Components/Drawing/AnimatorComponent.h"
#include "../../../Random.h"
#include "../../../Components/Physics/RigidBodyComponent.h"

WhiteBoss::WhiteBoss(class Game* game, Vector2 position, float forwardSpeed)
	: BossBase(game, position, forwardSpeed)
	, mFootstepTimer(0.0f)
{
	mAnimatorComponent = new AnimatorComponent(this, "WhiteBossAnim", GameConstants::TILE_SIZE * 2.0f, GameConstants::TILE_SIZE * 2.0f);

	auto data = LoadEnemyDataFromJSON("WhiteBossData");
	SetupAIBehaviors(data);
}

void WhiteBoss::OnUpdate(float deltaTime)
{
	EnemyBase::OnUpdate(deltaTime);

	// Play footstep sounds when moving
	if (mIsMoving && !mIsDead)
	{
		mFootstepTimer -= deltaTime;

		if (mFootstepTimer <= 0.0f)
		{
			mFootstepTimer = 0.4f;  // Reset timer (slower than player)

			// Use grass sounds for Boss 1
			std::string sound1 = "e10_boss_step_on_grass1.wav";
			std::string sound2 = "e11_boss_step_on_grass2.wav";

			// Play random one of the two sounds
			mGame->GetAudio()->PlaySound(rand() % 2 ? sound1 : sound2, false, 1.0f);
		}
	}
	else
	{
		mFootstepTimer = 0.0f;  // Reset when not moving
	}
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