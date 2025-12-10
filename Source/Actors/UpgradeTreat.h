#pragma once

#include "Actor.h"
#include "../Components/Physics/Collider.h"
#include "../Components/Physics/ColliderComponent.h"
#include "../Components/Drawing/AnimatorComponent.h"

class UpgradeTreat : public Actor
{
public:
	UpgradeTreat(class Game* game);
	
	void Awake(const Vector2& position);
	void Kill();
	
	void OnUpdate(float deltaTime) override;

	bool IsCollected() const { return mCollected; }

private:
	bool mCollected;
	ColliderComponent *mColliderComponent;
	AnimatorComponent *mAnimatorComponent;
};