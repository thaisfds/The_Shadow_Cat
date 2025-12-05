#include "Collider.h"
#include "ColliderComponent.h"
#include "Physics.h"
#include "../../Game.h"

Vector2 AABBCollider::GetMin() const
{
    Vector2 center = GetComponent()->GetPosition();
    return Vector2(center.x - mHalfDimensions.x, center.y - mHalfDimensions.y);
}

Vector2 AABBCollider::GetMax() const
{
    Vector2 center = GetComponent()->GetPosition();
    return Vector2(center.x + mHalfDimensions.x, center.y + mHalfDimensions.y);
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

bool AABBCollider::CheckCollision(const Collider* other) const
{
	if (this == other) return false;

	if (const AABBCollider* aabb = dynamic_cast<const AABBCollider*>(other))
		return Physics::CheckAABBAABB(this, aabb);
	else if (const CircleCollider* circle = dynamic_cast<const CircleCollider*>(other))
		return Physics::CheckAABBCircle(this, circle);

	return false;
}

bool CircleCollider::CheckCollision(const Collider* other) const
{
	if (this == other) return false;
	
	if (const CircleCollider* circle = dynamic_cast<const CircleCollider*>(other))
		return Physics::CheckCircleCircle(this, circle);
	else if (const AABBCollider* aabb = dynamic_cast<const AABBCollider*>(other))
		return Physics::CheckAABBCircle(aabb, this);

	return false;
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
					   Color::Green, mComponent->GetOwner()->GetGame()->GetCameraPos(), RendererMode::LINES);
}

void CircleCollider::DebugDraw(class Renderer* renderer)
{
	Vector2 center = mComponent->GetPosition();
	renderer->DrawCircle(center, mRadius, Color::Green,
						 mComponent->GetOwner()->GetGame()->GetCameraPos());
}