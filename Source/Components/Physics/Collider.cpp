#include "Collider.h"
#include "Physics.h"
#include "../../Game.h"
#include "../../LevelManager.h"

bool Collider::CheckCollision(const Collider* other)
{
	auto pos = mComponent->GetPosition();
	return CheckCollisionAt(&pos, other);
}

std::vector<ColliderComponent*> Collider::GetOverlappingColliders()
{
	auto pos = mComponent->GetPosition();
	return GetOverlappingCollidersAt(&pos);
}

Vector2 AABBCollider::GetMinAt(Vector2 position) const
{
	return Vector2(position.x - mHalfDimensions.x, position.y - mHalfDimensions.y);
}

Vector2 AABBCollider::GetMaxAt(Vector2 position) const
{
	return Vector2(position.x + mHalfDimensions.x, position.y + mHalfDimensions.y);
}

void AABBCollider::SolveCollisions(const RigidBodyComponent* rigidBody)
{
	auto overlappingColliders = Physics::GetOverlappingColliders(GetComponent()->GetGame(), this);

	for (auto other : overlappingColliders)
	{
		auto otherAABB = dynamic_cast<AABBCollider*>(other->GetCollider());
		if (!otherAABB) continue;

		float minXOverlap = GetMinHorizontalOverlap(otherAABB);
		float minYOverlap = GetMinVerticalOverlap(otherAABB);

		if (Math::Abs(minXOverlap) < Math::Abs(minYOverlap))
			ResolveHorizontalCollisions(const_cast<RigidBodyComponent*>(rigidBody), minXOverlap);
		else
			ResolveVerticalCollisions(const_cast<RigidBodyComponent*>(rigidBody), minYOverlap);
	}
}

bool AABBCollider::CheckCollisionAt(Vector2* newPosition, const Collider* other) const
{
	if (this == other) return false;

	if (const AABBCollider* aabb = dynamic_cast<const AABBCollider*>(other))
		return Physics::CheckAABBAABB(this, aabb, newPosition);
	else if (const CircleCollider* circle = dynamic_cast<const CircleCollider*>(other))
		return Physics::CheckAABBCircle(this, circle, newPosition);
	else if (const PolygonCollider* poly = dynamic_cast<const PolygonCollider*>(other))
		return Physics::CheckPolygonAABB(poly, this, nullptr, newPosition);

	return false;
}

bool CircleCollider::CheckCollisionAt(Vector2* newPosition, const Collider* other) const
{
	if (this == other) return false;
	
	if (const CircleCollider* circle = dynamic_cast<const CircleCollider*>(other))
		return Physics::CheckCircleCircle(this, circle, newPosition);
	else if (const AABBCollider* aabb = dynamic_cast<const AABBCollider*>(other))
		return Physics::CheckAABBCircle(aabb, this, nullptr, newPosition);
	else if (const PolygonCollider* poly = dynamic_cast<const PolygonCollider*>(other))
		SDL_Log("Circle-Polygon collision not implemented yet");

	return false;
}

bool PolygonCollider::CheckCollisionAt(Vector2* newPosition, const Collider* other) const
{
	if (this == other) return false;

	if (const PolygonCollider* poly = dynamic_cast<const PolygonCollider*>(other))
		return Physics::CheckPolygonPolygon(this, poly, newPosition);
	else if (const AABBCollider* aabb = dynamic_cast<const AABBCollider*>(other))
		return Physics::CheckPolygonAABB(this, aabb, newPosition);
	else if (const CircleCollider* circle = dynamic_cast<const CircleCollider*>(other))
		SDL_Log("Polygon-Circle collision not implemented yet");

	return false;
}

std::vector<ColliderComponent*> AABBCollider::GetOverlappingCollidersAt(Vector2* newPosition) const
{
	std::vector<ColliderComponent*> hitColliders;
	auto colliderComponents = LevelManager::Instance().GetColliders();
	CollisionFilter filter = mComponent->GetFilter();
	for (auto c : colliderComponents)
	{
		if (!c->IsEnabled()) continue;
		if (!CollisionFilter::ShouldCollide(filter, c->GetFilter())) continue;

		if (CheckCollisionAt(newPosition, c->GetCollider())) hitColliders.push_back(c);
	}

	return hitColliders;
}

std::vector<ColliderComponent*> CircleCollider::GetOverlappingCollidersAt(Vector2* newPosition) const
{
	std::vector<ColliderComponent*> hitColliders;
	auto colliderComponents = LevelManager::Instance().GetColliders();
	CollisionFilter filter = mComponent->GetFilter();
	for (auto c : colliderComponents)
	{
		if (!c->IsEnabled()) continue;
		if (!CollisionFilter::ShouldCollide(filter, c->GetFilter())) continue;

		if (CheckCollisionAt(newPosition, c->GetCollider())) hitColliders.push_back(c);
	}

	return hitColliders;
}

std::vector<ColliderComponent*> PolygonCollider::GetOverlappingCollidersAt(Vector2* newPosition) const
{
	std::vector<ColliderComponent*> hitColliders;
	auto colliderComponents = LevelManager::Instance().GetColliders();
	CollisionFilter filter = mComponent->GetFilter();
	for (auto c : colliderComponents)
	{
		if (!c->IsEnabled()) continue;
		if (!CollisionFilter::ShouldCollide(filter, c->GetFilter())) continue;

		if (CheckCollisionAt(newPosition, c->GetCollider())) hitColliders.push_back(c);
	}

	return hitColliders;
}

float AABBCollider::GetMinVerticalOverlap(AABBCollider *b) const
{
	Vector2 aMin = this->GetMin();
	Vector2 aMax = this->GetMax();
	Vector2 bMin = b->GetMin();
	Vector2 bMax = b->GetMax();

	float overlapTop = aMax.y - bMin.y;

	float overlapBottom = bMax.y - aMin.y;

	if (Math::Abs(overlapTop) < Math::Abs(overlapBottom))
		return overlapTop;
	else return -overlapBottom;
}

float AABBCollider::GetMinHorizontalOverlap(AABBCollider *b) const
{
	Vector2 aMin = this->GetMin();
	Vector2 aMax = this->GetMax();
	Vector2 bMin = b->GetMin();
	Vector2 bMax = b->GetMax();

	float overlapRight = aMax.x - bMin.x;

	float overlapLeft = bMax.x - aMin.x;

	if (Math::Abs(overlapRight) < Math::Abs(overlapLeft))
		return overlapRight;
	else return -overlapLeft;
}

void AABBCollider::ResolveHorizontalCollisions(RigidBodyComponent *rigidBody, const float minXOverlap)
{
	Vector2 position = mComponent->GetOwner()->GetPosition();
	position.x -= minXOverlap;
	mComponent->GetOwner()->SetPosition(position);

	Vector2 velocity = rigidBody->GetVelocity();
	velocity.x = 0.0f;
	rigidBody->SetVelocity(velocity);
}

void AABBCollider::ResolveVerticalCollisions(RigidBodyComponent *rigidBody, const float minYOverlap)
{
	Vector2 position = mComponent->GetOwner()->GetPosition();
	position.y -= minYOverlap;
	mComponent->GetOwner()->SetPosition(position);

	Vector2 velocity = rigidBody->GetVelocity();
	velocity.y = 0.0f;
	rigidBody->SetVelocity(velocity);
}

void AABBCollider::DebugDraw(class Renderer* renderer)
{
	Vector2 min = GetMin();
	Vector2 max = GetMax();
	Vector2 size = max - min;
	renderer->DrawRect(mComponent->GetPosition(), size, 0.0f,
					   Color::Green, LevelManager::Instance().GetCameraPos(), RendererMode::LINES);
}

void CircleCollider::DebugDraw(class Renderer* renderer)
{
	Vector2 center = mComponent->GetPosition();
	renderer->DrawCircle(center, mRadius, Color::Green,
						 LevelManager::Instance().GetCameraPos());
}

void PolygonCollider::DebugDraw(class Renderer* renderer)
{
	Vector2 offset = mComponent->GetPosition();
	renderer->DrawPolygon(mVertices, Color::Green, offset,
						 LevelManager::Instance().GetCameraPos());
}

void PolygonCollider::SetForward(Vector2 forward)
{
	mForward = forward;
	mForward.Normalize();
	
	// Recalculate vertices based on forward direction
	// Negate Y because coordinate system has Y increasing downward
	float angle = Math::Atan2(-mForward.y, mForward.x);
	Matrix2 rotation = Matrix2::CreateRotation(angle);
	
	mVertices.clear();
	for (const auto& baseVertex : mBaseVertices)
	{
		Vector2 rotated = rotation * baseVertex;
		mVertices.push_back(rotated);
	}
}