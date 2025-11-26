#include "Block.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

Block::Block(Game *game, const std::string &texturePath)
	: Actor(game)
{
	new AnimatorComponent(this, texturePath, "", Game::TILE_SIZE, Game::TILE_SIZE);
}

Block::Block(Game *game, int tileID)
	: Actor(game)
{
	auto animator = new AnimatorComponent(this, "../Assets/Levels/MapTileset.png",
										  "../Assets/Levels/MapTileset.json",
										  Game::TILE_SIZE, Game::TILE_SIZE);

	// The tileID corresponds directly to the frame index in the sprite sheet
	// MapTileset0 = index 0, MapTileset4 = index 4, etc.
	animator->AddAnimation("static", {tileID});
	animator->SetAnimation("static");
}
