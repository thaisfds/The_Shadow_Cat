#include "OrangeCat.h"
#include "../../../GameConstants.h"
#include "../../../AI/AIStateMachine.h"
#include "../../../Components/Skills/Dash.h"
#include "../../../Components/Drawing/AnimatorComponent.h"
#include "../ShadowCat.h"
#include "../../../LevelManager.h"
#include "../../../Components/Skills/FurBall.h"

OrangeCat::OrangeCat(Game* game, Vector2 position)
	: EnemyBase(game, position, 150.0f)
{
	mAnimatorComponent = new AnimatorComponent(this, "OrangeCatAnim", GameConstants::TILE_SIZE, GameConstants::TILE_SIZE);

	auto data = LoadEnemyDataFromJSON("OrangeCatData");

	for (auto skill : mSkills)
	{
		if (auto furBall = dynamic_cast<FurBall*>(skill))
		{
			furBall->ApplyUpgrade("range", 100);
			furBall->ApplyUpgrade("damage", 2);
			furBall->ApplyUpgrade("cooldown", 3);
			furBall->ApplyUpgrade("projectileSpeed", 2);
			furBall->SetAnimation("orange");
		}
	}
	
	SetupAIBehaviors(data);
}

void OrangeCat::SetupAIBehaviors(const nlohmann::json& data)
{
	mStateMachine = new AIStateMachine(this);

	SkillBase *dashSkill = nullptr;
	for (auto skill : mSkills)
	{
		if (auto dash = dynamic_cast<Dash*>(skill))
		{
			dashSkill = dash;
			break;
		}
	}

	mPatrolBehavior = new PatrolBehavior(this);
	mFleeBehavior = new FleeBehavior(this, dashSkill);
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

void OrangeCat::OnUpdate(float deltaTime)
{
	mFleeTimer -= deltaTime;
	EnemyBase::OnUpdate(deltaTime);
}