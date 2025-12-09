#include "Block.h"
#include "../Game.h"
#include "../GameConstants.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/ColliderComponent.h"
#include "../Components/Physics/Collider.h"
#include "../Components/Physics/CollisionFilter.h"

Block::Block(Game *game, const std::string &texturePath)
	: Actor(game)
{
	new AnimatorComponent(this, texturePath, GameConstants::TILE_SIZE, GameConstants::TILE_SIZE);

	// Create collider for block
	CollisionFilter filter;
	filter.belongsTo = CollisionFilter::GroupMask({CollisionGroup::Environment});
	filter.collidesWith = CollisionFilter::GroupMask({CollisionGroup::Player, CollisionGroup::Enemy, CollisionGroup::Environment});

	Collider *collider = new AABBCollider(GameConstants::TILE_SIZE, GameConstants::TILE_SIZE);
	new ColliderComponent(this, 0, 0, collider, filter, true);
}

Block::Block(Game *game, int tileID)
	: Actor(game)
{
	auto animator = new AnimatorComponent(this, "BlockAnim", GameConstants::TILE_SIZE, GameConstants::TILE_SIZE);
	animator->LoopAnimation(std::to_string(tileID));

	// Create collider for block
	CollisionFilter filter;
	filter.belongsTo = CollisionFilter::GroupMask({CollisionGroup::Environment});
	filter.collidesWith = CollisionFilter::GroupMask({CollisionGroup::Player, CollisionGroup::Enemy, CollisionGroup::Environment});

	Collider *collider = new AABBCollider(GameConstants::TILE_SIZE, GameConstants::TILE_SIZE);
	new ColliderComponent(this, 0, 0, collider, filter, true);
}
