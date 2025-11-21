#pragma once

#include "Actor.h"

class Block : public Actor
{
public:
	// Constructor for individual texture files (deprecated)
	explicit Block(Game *game, const std::string &texturePath);

	// Constructor for sprite sheet with frame index
	explicit Block(Game *game, int frameIndex);
};
