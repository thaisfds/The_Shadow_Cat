#pragma once

#include "../BaseEnemy.h"

class WhiteCat : public BaseEnemy {
public:
    WhiteCat(class Game* game, Vector2 position, float forwardSpeed = 200.0f);

protected:
    void SetupAIBehaviors() override;
};