#include "Block.h"
#include "../Game.h"
#include "../GameConstants.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/ColliderComponent.h"

Block::Block(Game *game, const std::string &texturePath)
	: Actor(game)
{
	new AnimatorComponent(this, texturePath, GameConstants::TILE_SIZE, GameConstants::TILE_SIZE);
}

Block::Block(Game *game, int tileID)
	: Actor(game)
{
	auto animator = new AnimatorComponent(this, "BlockAnim", GameConstants::TILE_SIZE, GameConstants::TILE_SIZE);
	animator->LoopAnimation(std::to_string(tileID));
}
