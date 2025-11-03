#pragma once

#include "Actor.h"

class Block : public Actor
{
public:
    explicit Block(Game *game, const std::string &texturePath);
};
