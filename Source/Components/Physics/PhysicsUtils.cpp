#include "PhysicsUtils.h"
#include "AABBColliderComponent.h"
#include "../../Game.h"
#include "../ParticleSystemComponent.h"

AABBColliderComponent* PhysicsUtils::ConeCast(Game *game, Vector2 origin, Vector2 direction, float angle, float radius, ColliderLayer layer)
{
	float radiusSq = radius * radius;
	auto colliders = game->GetColliders();
	for (auto collider : colliders)
	{
		if (collider->GetLayer() != layer) continue;
		if (PhysicsUtils::GetPointAABBDistanceSquared(origin, collider) > radiusSq) continue;

		if (OverlapTriangleAABB(origin, direction, angle, radius, collider)) return collider;
	}

	return nullptr;
}

bool PhysicsUtils::OverlapTriangleAABB(Vector2 origin, Vector2 direction, float angle, float length, AABBColliderComponent *aabb)
{
	// Create triangle vertices (cone shape)
	direction.Normalize();
	float halfAngle = angle / 2.0f;
	
	// Rotate direction by +halfAngle and -halfAngle to get cone edges
	Matrix2 rotationRight = Matrix2::CreateRotation(halfAngle);
	Matrix2 rotationLeft = Matrix2::CreateRotation(-halfAngle);
	
	// Right edge of triangle (rotated by +halfAngle)
	Vector2 rightDir = rotationRight * direction;
	
	// Left edge of triangle (rotated by -halfAngle)
	Vector2 leftDir = rotationLeft * direction;
	
	// Triangle vertices
	Vector2 v0 = origin; // Apex
	Vector2 v1 = origin + rightDir * length; // Right point
	Vector2 v2 = origin + leftDir * length;  // Left point
	
	// AABB vertices
	Vector2 aabbMin = aabb->GetMin();
	Vector2 aabbMax = aabb->GetMax();
	
	// Create polygon representations
	std::vector<Vector2> triangle = {v0, v1, v2};
	std::vector<Vector2> box = {
		aabbMin,
		Vector2(aabbMax.x, aabbMin.y),
		aabbMax,
		Vector2(aabbMin.x, aabbMax.y)
	};
	
	return OverlapPolygons(triangle, box);
}

bool PhysicsUtils::OverlapPolygons(const std::vector<Vector2> &polyA, const std::vector<Vector2> &polyB)
{
	// Using Separating Axis Theorem (SAT)
	auto edgesA = GetPolygonEdges(polyA);
	auto edgesB = GetPolygonEdges(polyB);

	for (const auto &edge : edgesA)
	{
		Vector2 axis(-edge.y, edge.x); // Perpendicular
		axis.Normalize();
		if (!PolygonsOverlapOnAxis(polyA, polyB, axis)) return false; // Found separating axis
	}
	
	for (const auto &edge : edgesB)
	{
		Vector2 axis(-edge.y, edge.x); // Perpendicular
		axis.Normalize();
		if (!PolygonsOverlapOnAxis(polyA, polyB, axis)) return false; // Found separating axis
	}
	
	return true; // No separating axis found, polygons overlap
}

float PhysicsUtils::GetPointAABBDistanceSquared(Vector2 point, AABBColliderComponent* aabb)
{
	// The closest point is where the straight line from the point to the center of the AABB intersects the AABB
	Vector2 aabbMin = aabb->GetMin();
	Vector2 aabbMax = aabb->GetMax();
	
	// Clamp the point to the AABB bounds on each axis
	float closestX = Math::Clamp(point.x, aabbMin.x, aabbMax.x);
	float closestY = Math::Clamp(point.y, aabbMin.y, aabbMax.y);
	
	// Calculate distance from point to closest point on AABB
	float dx = point.x - closestX;
	float dy = point.y - closestY;

	return dx * dx + dy * dy;
}
	
std::vector<Vector2> PhysicsUtils::GetPolygonEdges(const std::vector<Vector2> &poly)
{
	std::vector<Vector2> edges;
	size_t count = poly.size();
	for (size_t i = 0; i < count; ++i)
	{
		Vector2 edge = poly[(i + 1) % count] - poly[i];
		edges.push_back(edge);
	}
	return edges;
}

void PhysicsUtils::ProjectPolygonOntoAxis(const std::vector<Vector2> &poly, const Vector2 &axis, float &min, float &max)
{
	min = max = Vector2::Dot(poly[0], axis);
	for (const auto &vertex : poly)
	{
		float projection = Vector2::Dot(vertex, axis);
		if (projection < min) min = projection;
		if (projection > max) max = projection;
	}
}

bool PhysicsUtils::PolygonsOverlapOnAxis(const std::vector<Vector2> &polyA, const std::vector<Vector2> &polyB, const Vector2 &axis)
{
	float minA, maxA, minB, maxB;
	ProjectPolygonOntoAxis(polyA, axis, minA, maxA);
	ProjectPolygonOntoAxis(polyB, axis, minB, maxB);
	return !(maxA < minB || maxB < minA);
}