#pragma once

#include <initializer_list>

enum class CollisionGroup : int {
    None = 0,
    Player,
    Enemy,
    Environment,
    PlayerSkills,
    EnemySkills,
	Collectible,
};

struct CollisionFilter
{
	unsigned int belongsTo;
	unsigned int collidesWith;

	static unsigned int RemoveGroups(unsigned int mask, std::initializer_list<CollisionGroup> groups)
	{
		for (auto group : groups) mask &= ~(1 << static_cast<int>(group));
		return mask;
	}

	static unsigned int GroupMask(std::initializer_list<CollisionGroup> groups)
	{
		unsigned int mask = 0;
		for (auto group : groups) mask |= (1 << static_cast<int>(group));
		return mask;
	}

	static bool ShouldCollide(const CollisionFilter& a, const CollisionFilter& b)
	{
		return a.belongsTo & b.collidesWith && b.belongsTo & a.collidesWith;
	}
};