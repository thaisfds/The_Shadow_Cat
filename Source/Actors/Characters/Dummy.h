#pragma once
#include "Character.h"

class Dummy : public Character
{
public:
    Dummy(class Game* game, float forwardSpeed = 0.0f);
    virtual ~Dummy();
};