#pragma once
#include "../AIBehavior.h"

class AttackBehavior : public AIBehavior
{
public:
    AttackBehavior();
    
    void OnEnter(class Enemy* enemy) override;
    void Update(class Enemy* enemy, float deltaTime) override;
    void OnExit(class Enemy* enemy) override;
};
