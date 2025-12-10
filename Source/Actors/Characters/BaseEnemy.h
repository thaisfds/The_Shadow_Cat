#pragma once

#include "Character.h"
#include <vector>
#include "../../Components/Skills/SkillBase.h"

class BaseEnemy : public Character
{
public:
	BaseEnemy(class Game* game, Vector2 position, float forwardSpeed);
	virtual ~BaseEnemy();

	void OnUpdate(float deltaTime) override;
	void ResetCollisionFilter() const override;

protected:
	class AIStateMachine *mStateMachine;
	
	virtual void SetupAIBehaviors() = 0;

	nlohmann::json LoadEnemyDataFromJSON(const std::string& fileName);
};