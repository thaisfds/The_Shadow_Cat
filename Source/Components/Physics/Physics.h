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

	static bool CheckAABBAABB(const AABBCollider *a, const AABBCollider *b, Vector2 *positionA = nullptr, Vector2 *positionB = nullptr);
	static bool CheckAABBCircle(const AABBCollider *aabb, const CircleCollider *circle, Vector2 *positionA = nullptr, Vector2 *positionB = nullptr);
	static bool CheckCircleCircle(const CircleCollider *a, const CircleCollider *b, Vector2 *positionA = nullptr, Vector2 *positionB = nullptr);
	static bool CheckPolygonPolygon(const PolygonCollider *a, const PolygonCollider *b, Vector2 *positionA = nullptr, Vector2 *positionB = nullptr);
	static bool CheckPolygonAABB(const PolygonCollider* poly, const AABBCollider* aabb, Vector2* positionPoly = nullptr, Vector2* positionAABB = nullptr);

	static std::vector<ColliderComponent*> GetOverlappingColliders(Game* game, Collider* collider);
	static std::vector<ColliderComponent*> ConeCast(Game* game, Vector2 origin, Vector2 direction, float angle, float radius, CollisionFilter filter);

	static std::vector<ColliderComponent*> CheckCollisionAt(Game* game, Collider* collider, Vector2 newPosition, CollisionFilter filter);
	
	static bool OverlapCircleAABB(Vector2 center, float radius, const AABBCollider* aabb, Vector2* posAABB = nullptr);
	static bool OverlapTriangleCollider(const std::vector<Vector2>& triangle, const Collider* collider);
	static bool OverlapTriangleAABB(const std::vector<Vector2>& triangle, const AABBCollider* aabb);
	static bool OverlapPolygons(const std::vector<Vector2>& polyA, const std::vector<Vector2>& polyB);
	
	static std::vector<Vector2> GetConeVertices(Vector2 origin, Vector2 direction, float angle, float length);
	static float GetPointAABBDistanceSq(Vector2 point, const AABBCollider* aabb, Vector2* posAABB = nullptr);
	static bool IsColliderWithinDistance(Vector2 origin, const ColliderComponent* collider, float radius);
	static void DebugDrawPolygon(class Game* game, const std::vector<Vector2>& polygon, float lifetime, int particlesPerEdge = 20);
	static bool ShouldCollide(unsigned int belongsToA, unsigned int collidesWithA, unsigned int belongsToB, unsigned int collidesWithB);

private:
	static std::vector<Vector2> GetPolygonEdges(const std::vector<Vector2>& poly);
	static void ProjectPolygonOntoAxis(const std::vector<Vector2>& poly, const Vector2& axis, float& min, float& max);
	static bool PolygonsOverlapOnAxis(const std::vector<Vector2>& polyA, const std::vector<Vector2>& polyB, const Vector2& axis);
};