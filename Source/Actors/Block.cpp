#include "Block.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

Block::Block(Game *game, const std::string &texturePath)
    : Actor(game)
{
    new AnimatorComponent(this, texturePath, "", Game::TILE_SIZE, Game::TILE_SIZE);
}
