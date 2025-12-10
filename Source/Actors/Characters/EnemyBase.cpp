#include "EnemyBase.h"
#include "../../AI/AIStateMachine.h"
#include "../../Components/Physics/RigidBodyComponent.h"
#include "../../SkillFactory.h"
#include <fstream>
#include "../../Game.h"
#include "../../Random.h"

EnemyBase::EnemyBase(class Game* game, Vector2 position, float forwardSpeed)
	: Character(game, position, forwardSpeed)
	, mStateMachine(nullptr)
{
	mGame->RegisterEnemy(this);
	mSkillFilter.belongsTo = CollisionFilter::GroupMask({ CollisionGroup::EnemySkills });
	mSkillFilter.collidesWith = CollisionFilter::GroupMask({ CollisionGroup::Player });
	ResetCollisionFilter();
}

EnemyBase::~EnemyBase()
{
	delete mStateMachine;
}

void EnemyBase::Kill()
{
	mGame->UnregisterEnemy(this);

	if (Random::GetFloatRange(0.0f, 1.0f) <= mUpgradeDropChance)
	{
		auto upgradeTreatActor = mGame->GetUpgradeTreatActor();
		upgradeTreatActor->Awake(mPosition);
	}
	
	Character::Kill();
}

void EnemyBase::OnUpdate(float deltaTime)
{
	Character::OnUpdate(deltaTime);
	
	if (mIsDead) return;
	
	if (mStateMachine) mStateMachine->Update(deltaTime);
}

void EnemyBase::ResetCollisionFilter() const
{
	CollisionFilter filter;
	filter.belongsTo = CollisionFilter::GroupMask({ CollisionGroup::Enemy });
	filter.collidesWith = CollisionFilter::GroupMask({ CollisionGroup::Player, CollisionGroup::Enemy, CollisionGroup::PlayerSkills, CollisionGroup::Environment });
	mColliderComponent->SetFilter(filter);
}

nlohmann::json EnemyBase::LoadEnemyDataFromJSON(const std::string& fileName)
{
	std::ifstream enemyFile("../Assets/Data/Character/" + fileName + ".json");

	if (!enemyFile.is_open())
	{
		SDL_Log("Failed to open enemy file: %s", (fileName).c_str());
		return nullptr;
	}

	nlohmann::json enemyData = nlohmann::json::parse(enemyFile);
	if (enemyData.is_null())
	{
		SDL_Log("Failed to parse enemy file: %s", (fileName).c_str());
		return nullptr;
	}

	hp = GameJsonParser::GetIntValue(enemyData, "hp");
	mForwardSpeed = GameJsonParser::GetFloatValue(enemyData, "speed");
	mUpgradeDropChance = GameJsonParser::GetValue<float>(enemyData, "dropChance", 0.1f);
	auto skills = GameJsonParser::GetStringArrayValue(enemyData, "skills");
	for (const auto& skillID : skills)
		mSkills.push_back(SkillFactory::Instance().CreateSkill(skillID, this));

	return enemyData;
}