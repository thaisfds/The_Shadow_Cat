#pragma once

#include "Character.h"
#include <vector>
#include "../../Components/Skills/SkillBase.h"

class EnemyBase : public Character
{
public:
	EnemyBase(class Game* game, Vector2 position, float forwardSpeed);
	virtual ~EnemyBase();

	void OnUpdate(float deltaTime) override;
	void ResetCollisionFilter() const override;

protected:
	class AIStateMachine *mStateMachine;
	
	virtual void SetupAIBehaviors() = 0;

	nlohmann::json LoadEnemyDataFromJSON(const std::string& fileName);
};