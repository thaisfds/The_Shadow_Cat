#pragma once
#include "../../Math.h"
#include <vector>

enum class ColliderLayer;
class AABBColliderComponent;
class Game;

class PhysicsUtils
{
public:
	PhysicsUtils() = delete; // Prevent instantiation

	static float GetPointAABBDistanceSquared(Vector2 point, AABBColliderComponent* aabb);
	static AABBColliderComponent* ConeCast(Game* game, Vector2 origin, Vector2 direction, float angle, float radius, ColliderLayer layer);

	static bool OverlapTriangleAABB(Vector2 origin, Vector2 direction, float angle, float radius, AABBColliderComponent* aabb);
 	static bool OverlapPolygons(const std::vector<Vector2>& polyA, const std::vector<Vector2>& polyB);

private:
	static std::vector<Vector2> GetPolygonEdges(const std::vector<Vector2>& poly);
	static void ProjectPolygonOntoAxis(const std::vector<Vector2>& poly, const Vector2& axis, float& min, float& max);
	static bool PolygonsOverlapOnAxis(const std::vector<Vector2>& polyA, const std::vector<Vector2>& polyB, const Vector2& axis);
};