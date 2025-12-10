#pragma once
#include "Character.h"
#include "../../Components/Physics/ColliderComponent.h"

class Dummy : public Character
{
public:
    Dummy(class Game* game, Vector2 position, float forwardSpeed = 0.0f);
    virtual ~Dummy();

    void TakeDamage(int damage) override;

    void ResetCollisionFilter() const override { mColliderComponent->SetFilter(GetBaseEnemyFilter()); }
};