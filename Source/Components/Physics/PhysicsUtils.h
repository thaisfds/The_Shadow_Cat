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

	static std::vector<AABBColliderComponent*> ConeCast(Game* game, Vector2 origin, Vector2 direction, float angle, float radius, ColliderLayer layer);
	
	static bool OverlapTriangleAABB(const std::vector<Vector2>& triangle, AABBColliderComponent* aabb);
	static bool OverlapPolygons(const std::vector<Vector2>& polyA, const std::vector<Vector2>& polyB);
	
	static std::vector<Vector2> CreateConeTriangle(Vector2 origin, Vector2 direction, float angle, float length);
	
	static float GetPointAABBDistanceSquared(Vector2 point, AABBColliderComponent* aabb);
	
	static void DebugDrawPolygon(class Game* game, const std::vector<Vector2>& polygon, float lifetime, int particlesPerEdge = 20);

private:
	static std::vector<Vector2> GetPolygonEdges(const std::vector<Vector2>& poly);
	static void ProjectPolygonOntoAxis(const std::vector<Vector2>& poly, const Vector2& axis, float& min, float& max);
	static bool PolygonsOverlapOnAxis(const std::vector<Vector2>& polyA, const std::vector<Vector2>& polyB, const Vector2& axis);
};