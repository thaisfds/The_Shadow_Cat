#pragma once
#include "../../Math.h"
#include "CollisionFilter.h"
#include "Collider.h"
#include <vector>

class ColliderComponent;
class Game;

class Physics
{
public:
	Physics() = delete; // Prevent instantiation

	static bool CheckAABBAABB(const AABBCollider *a, const AABBCollider *b);
	static bool CheckAABBCircle(const AABBCollider *aabb, const CircleCollider *circle);
	static bool CheckCircleCircle(const CircleCollider *a, const CircleCollider *b);

	static std::vector<ColliderComponent*> GetOverlappingColliders(Game* game, Collider* collider);
	static std::vector<ColliderComponent*> ConeCast(Game* game, Vector2 origin, Vector2 direction, float angle, float radius, CollisionFilter filter);
	static bool OverlapCircleAABB(Vector2 center, float radius, const AABBCollider* aabb);
	static bool OverlapTriangleCollider(const std::vector<Vector2>& triangle, const Collider* collider);
	static bool OverlapTriangleAABB(const std::vector<Vector2>& triangle, const AABBCollider* aabb);
	static bool OverlapPolygons(const std::vector<Vector2>& polyA, const std::vector<Vector2>& polyB);
	static std::vector<Vector2> CreateConeTriangle(Vector2 origin, Vector2 direction, float angle, float length);
	static float GetPointAABBDistanceSquared(Vector2 point, const AABBCollider* aabb);
	static bool IsColliderWithinDistance(Vector2 origin, const ColliderComponent* collider, float radius);
	static void DebugDrawPolygon(class Game* game, const std::vector<Vector2>& polygon, float lifetime, int particlesPerEdge = 20);
	static bool ShouldCollide(unsigned int belongsToA, unsigned int collidesWithA, unsigned int belongsToB, unsigned int collidesWithB);

private:
	static std::vector<Vector2> GetPolygonEdges(const std::vector<Vector2>& poly);
	static void ProjectPolygonOntoAxis(const std::vector<Vector2>& poly, const Vector2& axis, float& min, float& max);
	static bool PolygonsOverlapOnAxis(const std::vector<Vector2>& polyA, const std::vector<Vector2>& polyB, const Vector2& axis);
};