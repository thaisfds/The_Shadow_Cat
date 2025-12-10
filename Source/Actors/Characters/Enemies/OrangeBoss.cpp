#include "OrangeBoss.h"
#include "../../../GameConstants.h"
#include "../../../AI/AIStateMachine.h"
#include "../../../AI/Behaviors/PatrolBehavior.h"
#include "../../../AI/Behaviors/ChaseBehavior.h"
#include "../../../AI/Behaviors/SkillBehavior.h"
#include "../../../Components/Drawing/AnimatorComponent.h"
#include "../../../Components/Skills/BossHealing.h"
#include "../../../Actors/Characters/ShadowCat.h"
#include "../../../Random.h"
#include "../../../Components/Physics/Collider.h"

OrangeBoss::OrangeBoss(class Game* game, Vector2 position, float forwardSpeed)
	: BossBase(game, position, forwardSpeed)
	, mFootstepTimer(0.0f)
{
	mAnimatorComponent = new AnimatorComponent(this, "OrangeBossAnim", GameConstants::TILE_SIZE * 2.0f, GameConstants::TILE_SIZE * 2.0f);

	// Boss sprite is 128x128 (TILE_SIZE * 2.0f), so we need a larger collider
	// Replace the default Character collider (48x32) with a boss-sized one (112x96)
	// This matches the size used in the old Boss class
	Collider *oldCollider = mColliderComponent->GetCollider();
	if (oldCollider)
	{
		delete oldCollider;
	}
	Collider *bossCollider = new AABBCollider(112, 96);
	mColliderComponent->SetCollider(bossCollider);

	// Load additional animations for skills
	// Note: WhiteSlash and WhiteBomb use particle/projectile systems, not boss animations
	// BossHealing animation will be loaded when needed

	auto data = LoadEnemyDataFromJSON("OrangeBossData");
	SetupAIBehaviors(data);
}

void OrangeBoss::OnUpdate(float deltaTime)
{
	EnemyBase::OnUpdate(deltaTime);

	if (mIsDead)
		return;

	// Check if healing should trigger
	CheckAndTriggerHealing();

	// Play footstep sounds when moving
	if (mIsMoving && !mIsDead)
	{
		mFootstepTimer -= deltaTime;

		if (mFootstepTimer <= 0.0f)
		{
			mFootstepTimer = 0.4f; // Reset timer (slower than player)

			// Use grass sounds for Orange Boss
			std::string sound1 = "e10_boss_step_on_grass1.wav";
			std::string sound2 = "e11_boss_step_on_grass2.wav";

			// Play random one of the two sounds
			GetGame()->GetAudio()->PlaySound(rand() % 2 ? sound1 : sound2, false, 1.0f);
		}
	}
	else
	{
		mFootstepTimer = 0.0f; // Reset when not moving
	}
}

void OrangeBoss::CheckAndTriggerHealing()
{
	// Find BossHealing skill
	BossHealing *healingSkill = nullptr;
	for (auto skill : mSkills)
	{
		healingSkill = dynamic_cast<BossHealing *>(skill);
		if (healingSkill)
			break;
	}

	if (!healingSkill)
		return;

	// Check if healing should trigger
	if (healingSkill->ShouldTriggerHealing())
	{
		// Get player position for skill target (healing doesn't need it, but StartSkill requires it)
		auto player = GetGame()->GetPlayer();
		Vector2 targetPos = player ? player->GetPosition() : GetPosition();
		healingSkill->StartSkill(targetPos);
	}
}

void OrangeBoss::SetupAIBehaviors(const nlohmann::json& data)
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

	// Improved transitions following WhiteCat pattern
	patrol->AddTransition(chase->GetName(), [patrol]() { return patrol->PatrolToChase(); });
	chase->AddTransition(patrol->GetName(), [chase]() { return chase->ChaseToPatrol(); });
	chase->AddTransition(skill->GetName(), [skill]() { return skill->AnySkillAvailable(); });
	skill->AddTransition(patrol->GetName(), [skill]() { return skill->SkillToPatrol(); });
	skill->AddTransitionCondition([skill]() { return skill->AnySkillAvailable(); }); // Allow re-entering skill state when skills available

	mStateMachine->SetInitialState(patrol->GetName());
}
