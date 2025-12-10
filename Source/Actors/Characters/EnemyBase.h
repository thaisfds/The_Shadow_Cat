#pragma once

#include "Character.h"
#include <vector>
#include "../../Components/Skills/SkillBase.h"
#include "../../Json.h"

class EnemyBase : public Character
{
public:
	EnemyBase(class Game* game, Vector2 position, float forwardSpeed);
	virtual ~EnemyBase();

	void OnUpdate(float deltaTime) override;
	void ResetCollisionFilter() const override;

	void Kill() override;

protected:
	class AIStateMachine *mStateMachine;
	
	virtual void SetupAIBehaviors(const nlohmann::json& data) = 0;

	nlohmann::json LoadEnemyDataFromJSON(const std::string& fileName);
};