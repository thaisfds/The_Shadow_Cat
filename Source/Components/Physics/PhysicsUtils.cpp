#include "PhysicsUtils.h"
#include "AABBColliderComponent.h"
#include "../../Game.h"
#include "../ParticleSystemComponent.h"
#include "CollisionFilter.h"

std::vector<AABBColliderComponent*> PhysicsUtils::ConeCast(Game *game, Vector2 origin, Vector2 direction, float angle, float radius, CollisionFilter filter)
{
	float radiusSq = radius * radius;
	auto colliders = game->GetColliders();
	std::vector<Vector2> coneTriangle = CreateConeTriangle(origin, direction, angle, radius);
	ParticleSystemComponent* debugParticles = game->GetDebugActor()->GetParticleSystemComponent();
	std::vector<AABBColliderComponent*> hitColliders;
	for (auto collider : colliders)
	{
		if (!CollisionFilter::ShouldCollide(filter, collider->GetFilter())) continue;
		if (GetPointAABBDistanceSquared(origin, collider) > radiusSq) continue;

		if (OverlapTriangleAABB(coneTriangle, collider))
			hitColliders.push_back(collider);
	}

	if (game->IsDebugging()) DebugDrawPolygon(game, coneTriangle, 0.5f, 15);
	return hitColliders;
}

bool PhysicsUtils::OverlapCircleAABB(Vector2 center, float radius, AABBColliderComponent *aabb)
{
	float radiusSq = radius * radius;
	float distSq = GetPointAABBDistanceSquared(center, aabb);
	return distSq <= radiusSq;
}

bool PhysicsUtils::OverlapTriangleAABB(const std::vector<Vector2>& triangle, AABBColliderComponent *aabb)
{
	Vector2 aabbMin = aabb->GetMin();
	Vector2 aabbMax = aabb->GetMax();
	std::vector<Vector2> box = {
		aabbMin,
		Vector2(aabbMax.x, aabbMin.y),
		aabbMax,
		Vector2(aabbMin.x, aabbMax.y)
	};
	bool overlap = OverlapPolygons(triangle, box);

	return overlap;
}

std::vector<Vector2> PhysicsUtils::CreateConeTriangle(Vector2 origin, Vector2 direction, float angle, float length)
{
	direction.Normalize();
	float halfAngle = angle / 2.0f;

	Matrix2 rotationRight = Matrix2::CreateRotation(halfAngle);
	Matrix2 rotationLeft = Matrix2::CreateRotation(-halfAngle);

	Vector2 rightDir = rotationRight * direction;
	Vector2 leftDir = rotationLeft * direction;

	Vector2 v0 = origin;
	Vector2 v1 = origin + rightDir * length;
	Vector2 v2 = origin + leftDir * length;
	
	return {v0, v1, v2};
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
	// The closest point is it hits going straight to the AABB's center
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


void PhysicsUtils::DebugDrawPolygon(Game* game, const std::vector<Vector2>& polygon, float lifetime, int particlesPerEdge)
{
	auto particleSystem = game->GetDebugActor()->GetParticleSystemComponent();
    if (!particleSystem || polygon.size() < 2) return;
    for (size_t i = 0; i < polygon.size(); ++i)
    {
        const Vector2& start = polygon[i];
        const Vector2& end = polygon[(i + 1) % polygon.size()];
        Vector2 edge = end - start;
        for (int j = 0; j <= particlesPerEdge; ++j)
        {
            float t = static_cast<float>(j) / static_cast<float>(particlesPerEdge);
            Vector2 pos = start + edge * t;
            particleSystem->EmitParticle(lifetime, 0.0f, pos - particleSystem->GetOwner()->GetPosition());
        }
    }
}