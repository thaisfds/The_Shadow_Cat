#pragma once

#include "../EnemyBase.h"

class WhiteCat : public EnemyBase {
public:
    WhiteCat(class Game* game, Vector2 position, float forwardSpeed = 200.0f);

protected:
    void SetupAIBehaviors() override;
};