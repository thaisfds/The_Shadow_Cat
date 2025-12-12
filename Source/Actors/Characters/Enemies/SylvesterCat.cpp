#include "SylvesterCat.h"
#include "../../../GameConstants.h"
#include "../../../AI/AIStateMachine.h"
#include "../../../Components/Skills/Dash.h"
#include "../ShadowCat.h"
#include "../../../LevelManager.h"
#include "../../../Components/Skills/ShadowForm.h"
#include "../../../Components/Drawing/AnimatorComponent.h"
#include "../../../Components/Skills/Stomp.h"

SylvesterCat::SylvesterCat(Game* game, Vector2 position)
	: EnemyBase(game, position, 150.0f)
{
	mAnimatorComponent = new AnimatorComponent(this, "SylvesterCatAnim", GameConstants::TILE_SIZE, GameConstants::TILE_SIZE);

	auto data = LoadEnemyDataFromJSON("SylvesterCatData");

	for (auto skill : mSkills)
	{
		if (auto shadowForm = dynamic_cast<ShadowForm*>(skill))
		{
			shadowForm->ApplyUpgrade("duration", -4);
			shadowForm->ApplyUpgrade("speedMultiplier", 5);
			shadowForm->ApplyUpgrade("cooldown", 14);
		}
		if (auto stomp = dynamic_cast<Stomp*>(skill))
		{
			stomp->ApplyUpgrade("damage", 3);
			stomp->ApplyUpgrade("radius", 3);
			stomp->ApplyUpgrade("range", 100);
			stomp->ApplyUpgrade("cooldown", 2);
		}
	}
	
	SetupAIBehaviors(data);
}

void SylvesterCat::SetupAIBehaviors(const nlohmann::json& data)
{
	mStateMachine = new AIStateMachine(this);

	SkillBase *shadowFormSkill = nullptr;
	for (auto skill : mSkills)
		if (auto shadowForm = dynamic_cast<ShadowForm*>(skill))
			shadowFormSkill = shadowForm;

	mPatrolBehavior = new PatrolBehavior(this);
	mFleeBehavior = new FleeBehavior(this, shadowFormSkill);
	mSkillBehavior = new SkillBehavior(this);

	mPatrolBehavior->LoadBehaviorData(data);
	mFleeBehavior->LoadBehaviorData(data);
	mSkillBehavior->LoadBehaviorData(data);

	mStateMachine->RegisterState(mPatrolBehavior);
	mStateMachine->RegisterState(mFleeBehavior);
	mStateMachine->RegisterState(mSkillBehavior);

	mPatrolBehavior->AddTransition(mFleeBehavior->GetName(), [this]() 
	{
		if (mFleeTimer > 0.0f) return false;
		float distanceToPlayer = (LevelManager::Instance().GetPlayer()->GetPosition() - mPosition).Length();
		return distanceToPlayer <= mFleeBehavior->GetFleeDistance() / 2.0f; 
	});

	mFleeBehavior->AddTransition(mSkillBehavior->GetName(), [this]() 
	{
		return mFleeBehavior->ShouldLeaveState() && !mIsUsingSkill;
	});
	mSkillBehavior->AddTransition(mPatrolBehavior->GetName(), [this]() { return !mIsUsingSkill; });

	mStateMachine->SetInitialState(mPatrolBehavior->GetName());
}

void SylvesterCat::OnUpdate(float deltaTime)
{
	mFleeTimer -= deltaTime;
	EnemyBase::OnUpdate(deltaTime);
}