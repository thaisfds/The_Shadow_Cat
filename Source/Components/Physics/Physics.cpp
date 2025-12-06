#include "Physics.h"
#include "ColliderComponent.h"
#include "../../Game.h"
#include "../ParticleSystemComponent.h"
#include "CollisionFilter.h"
#include "Collider.h"
#include <cmath>
#include <algorithm>
#include <vector>

bool Physics::CheckAABBAABB(const AABBCollider *a, const AABBCollider *b, Vector2 *positionA, Vector2 *positionB)
{
	auto posA = positionA ? *positionA : a->GetComponent()->GetPosition();
	auto halfA = a->GetHalfDimensions();
	auto posB = positionB ? *positionB : b->GetComponent()->GetPosition();
	auto halfB = b->GetHalfDimensions();

	return (Math::Abs(posA.x - posB.x) <= (halfA.x + halfB.x)) &&
		   (Math::Abs(posA.y - posB.y) <= (halfA.y + halfB.y));
}

bool Physics::CheckAABBCircle(const AABBCollider *aabb, const CircleCollider *circle, Vector2 *positionA, Vector2 *positionB)
{
	auto posCircle = positionB ? *positionB : circle->GetComponent()->GetPosition();

	return OverlapCircleAABB(posCircle, circle->GetRadius(), aabb, positionA);
}

bool Physics::CheckCircleCircle(const CircleCollider* a, const CircleCollider* b, Vector2* positionA, Vector2* positionB)
{
	auto posA = positionA ? *positionA : a->GetComponent()->GetPosition();
	auto posB = positionB ? *positionB : b->GetComponent()->GetPosition();

	float dx = posA.x - posB.x;
	float dy = posA.y - posB.y;
	float rSum = a->GetRadius() + b->GetRadius();
	return (dx * dx + dy * dy) <= (rSum * rSum);
}

bool Physics::CheckPolygonPolygon(const PolygonCollider* a, const PolygonCollider* b, Vector2* positionA, Vector2* positionB)
{
	auto posA = positionA ? *positionA : a->GetComponent()->GetPosition();
	auto posB = positionB ? *positionB : b->GetComponent()->GetPosition();

	std::vector<Vector2> polyA;
	for (const auto& vertex : a->GetVertices())
		polyA.push_back(vertex + posA);

	std::vector<Vector2> polyB;
	for (const auto& vertex : b->GetVertices())
		polyB.push_back(vertex + posB);

	return OverlapPolygons(polyA, polyB);
}

bool Physics::CheckPolygonAABB(const PolygonCollider* poly, const AABBCollider* aabb, Vector2* positionPoly, Vector2* positionAABB)
{
	auto posPoly = positionPoly ? *positionPoly : poly->GetComponent()->GetPosition();
	auto posAABB = positionAABB ? *positionAABB : aabb->GetComponent()->GetPosition();

	std::vector<Vector2> polygon;
	for (const auto& vertex : poly->GetVertices())
		polygon.push_back(vertex + posPoly);

	Vector2 aabbMin = aabb->GetMinAt(posAABB);
	Vector2 aabbMax = aabb->GetMaxAt(posAABB);
	std::vector<Vector2> box = {
		aabbMin,
		Vector2(aabbMax.x, aabbMin.y),
		aabbMax,
		Vector2(aabbMin.x, aabbMax.y)
	};

	return OverlapPolygons(polygon, box);
}

std::vector<ColliderComponent*> Physics::GetOverlappingColliders(Game* game, Collider* collider)
{
	std::vector<ColliderComponent*> hitColliders;
	auto colliderComponents = game->GetColliders();
	CollisionFilter filter = collider->GetComponent()->GetFilter();
	for (auto c : colliderComponents)
	{
		if (!c->IsEnabled()) continue;
		if (!CollisionFilter::ShouldCollide(filter, c->GetFilter())) continue;

		if (collider->CheckCollision(c->GetCollider())) hitColliders.push_back(c);
	}

	return hitColliders;
}

std::vector<ColliderComponent*> Physics::ConeCast(Game *game, Vector2 origin, Vector2 direction, float angle, float radius, CollisionFilter filter)
{
	float radiusSq = radius * radius;
	auto colliderComponents = game->GetColliders();
	std::vector<Vector2> coneTriangle = GetConeVertices(origin, direction, angle, radius);
	std::vector<ColliderComponent*> hitColliders;
	for (auto c : colliderComponents)
	{
		if (!c->IsEnabled()) continue;
		if (!CollisionFilter::ShouldCollide(filter, c->GetFilter())) continue;
		if (!IsColliderWithinDistance(origin, c, radius)) continue;

		if (OverlapTriangleCollider(coneTriangle, c->GetCollider())) hitColliders.push_back(c);
	}

	if (game->IsDebugging()) DebugDrawPolygon(game, coneTriangle, 0.5f, 15);
	return hitColliders;
}

std::vector<ColliderComponent*> Physics::CheckCollisionAt(Game* game, Collider* collider, Vector2 newPosition, CollisionFilter filter)
{
	std::vector<ColliderComponent*> hitColliders;
	auto colliderComponents = game->GetColliders();
	for (auto c : colliderComponents)
	{
		if (!c->IsEnabled()) continue;
		if (!CollisionFilter::ShouldCollide(filter, c->GetFilter())) continue;
		if (collider->CheckCollision(c->GetCollider()))
		{
			hitColliders.push_back(c);
		}
	}

	return hitColliders;
}

bool Physics::OverlapCircleAABB(Vector2 center, float radius, const AABBCollider *aabb, Vector2* posAABB)
{
	float radiusSq = radius * radius;
	float distSq = GetPointAABBDistanceSq(center, aabb, posAABB);
	return distSq <= radiusSq;
}

bool Physics::OverlapTriangleCollider(const std::vector<Vector2>& triangle, const Collider* collider)
{
	if (auto aabb = dynamic_cast<const AABBCollider*>(collider))
		return OverlapTriangleAABB(triangle, aabb);
	else if (auto circle = dynamic_cast<const CircleCollider*>(collider))
		return false;
	return false;
}

bool Physics::OverlapTriangleAABB(const std::vector<Vector2>& triangle, const AABBCollider *aabb)
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

std::vector<Vector2> Physics::GetConeVertices(Vector2 origin, Vector2 direction, float angle, float length)
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

bool Physics::OverlapPolygons(const std::vector<Vector2> &polyA, const std::vector<Vector2> &polyB)
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

bool Physics::IsColliderWithinDistance(Vector2 origin, const ColliderComponent* collider, float radius)
{
    const Collider* baseCollider = collider->GetCollider();
    if (const AABBCollider* aabb = dynamic_cast<const AABBCollider*>(baseCollider)) 
		return GetPointAABBDistanceSq(origin, aabb) <= (radius * radius);
    else if (const CircleCollider* circle = dynamic_cast<const CircleCollider*>(baseCollider)) {
        Vector2 center = collider->GetPosition();
        float dx = origin.x - center.x;
        float dy = origin.y - center.y;
        return (dx * dx + dy * dy) <= (radius * radius);
    }
    return false;
}

float Physics::GetPointAABBDistanceSq(Vector2 point, const AABBCollider* aabb, Vector2* posAABB)
{
	Vector2 aabbMin = aabb->GetMinAt(posAABB ? *posAABB : aabb->GetComponent()->GetPosition());
	Vector2 aabbMax = aabb->GetMaxAt(posAABB ? *posAABB : aabb->GetComponent()->GetPosition());
	float closestX = Math::Clamp(point.x, aabbMin.x, aabbMax.x);
	float closestY = Math::Clamp(point.y, aabbMin.y, aabbMax.y);
	float dx = point.x - closestX;
	float dy = point.y - closestY;
	return dx * dx + dy * dy;
}
	
std::vector<Vector2> Physics::GetPolygonEdges(const std::vector<Vector2> &poly)
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

void Physics::ProjectPolygonOntoAxis(const std::vector<Vector2> &poly, const Vector2 &axis, float &min, float &max)
{
	min = max = Vector2::Dot(poly[0], axis);
	for (const auto &vertex : poly)
	{
		float projection = Vector2::Dot(vertex, axis);
		if (projection < min) min = projection;
		if (projection > max) max = projection;
	}
}

bool Physics::PolygonsOverlapOnAxis(const std::vector<Vector2> &polyA, const std::vector<Vector2> &polyB, const Vector2 &axis)
{
	float minA, maxA, minB, maxB;
	ProjectPolygonOntoAxis(polyA, axis, minA, maxA);
	ProjectPolygonOntoAxis(polyB, axis, minB, maxB);
	return !(maxA < minB || maxB < minA);
}

void Physics::DebugDrawPolygon(Game* game, const std::vector<Vector2>& polygon, float lifetime, int particlesPerEdge)
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