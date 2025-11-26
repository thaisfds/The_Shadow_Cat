//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "AABBColliderComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"

AABBColliderComponent::AABBColliderComponent(class Actor *owner, int dx, int dy, int w, int h,
											 ColliderLayer layer, bool isStatic, int updateOrder)
	: Component(owner, updateOrder)
	  , mOffset(Vector2((float) dx, (float) dy))
	  , mIsStatic(isStatic)
	  , mWidth(w)
	  , mHeight(h)
	  , mLayer(layer)
{
	GetGame()->AddCollider(this);
}

AABBColliderComponent::~AABBColliderComponent()
{
	GetGame()->RemoveCollider(this);
}

Vector2 AABBColliderComponent::GetMin() const
{
	Vector2 center = mOwner->GetPosition() + mOffset;
	return Vector2(center.x - mWidth / 2.0f, center.y - mHeight / 2.0f);
}

Vector2 AABBColliderComponent::GetMax() const
{
	Vector2 center = mOwner->GetPosition() + mOffset;
	return Vector2(center.x + mWidth / 2.0f, center.y + mHeight / 2.0f);
}

bool AABBColliderComponent::Intersect(const AABBColliderComponent &b) const
{
	Vector2 aMin = this->GetMin();
	Vector2 aMax = this->GetMax();
	Vector2 bMin = b.GetMin();
	Vector2 bMax = b.GetMax();

	bool overlapX = (aMin.x < bMax.x) && (aMax.x > bMin.x);
	bool overlapY = (aMin.y < bMax.y) && (aMax.y > bMin.y);

	return overlapX && overlapY;
}

float AABBColliderComponent::GetMinVerticalOverlap(AABBColliderComponent *b) const
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

float AABBColliderComponent::GetMinHorizontalOverlap(AABBColliderComponent *b) const
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

float AABBColliderComponent::DetectHorizontalCollision(RigidBodyComponent *rigidBody)
{
	if (mIsStatic) return 0.0f;

	auto colliders = GetGame()->GetColliders();

	float minOverlap = 0.0f;

	for (auto other: colliders)
	{
		if (other == this) continue;
		if (!other->IsEnabled()) continue;

		if (Intersect(*other))
		{
			float overlap = GetMinHorizontalOverlap(other);
			ResolveHorizontalCollisions(rigidBody, overlap);
			mOwner->OnHorizontalCollision(overlap, other);
			minOverlap = overlap;
		}
	}

	return minOverlap;
}

float AABBColliderComponent::DetectVertialCollision(RigidBodyComponent *rigidBody)
{
	if (mIsStatic) return 0.0f;

	auto colliders = GetGame()->GetColliders();

	float minOverlap = 0.0f;
	bool hasCollision = false;

	for (auto other: colliders)
	{
		if (other == this) continue;
		if (!other->IsEnabled()) continue;

		if (Intersect(*other))
		{
			float overlap = GetMinVerticalOverlap(other);
			ResolveVerticalCollisions(rigidBody, overlap);
			mOwner->OnVerticalCollision(overlap, other);
			minOverlap = overlap;
			hasCollision = true;
		}
	}

	if (!hasCollision) mOwner->SetOffGround();

	return minOverlap;
}

void AABBColliderComponent::ResolveHorizontalCollisions(RigidBodyComponent *rigidBody, const float minXOverlap)
{
	Vector2 position = mOwner->GetPosition();
	position.x -= minXOverlap;
	mOwner->SetPosition(position);

	Vector2 velocity = rigidBody->GetVelocity();
	velocity.x = 0.0f;
	rigidBody->SetVelocity(velocity);
}

void AABBColliderComponent::ResolveVerticalCollisions(RigidBodyComponent *rigidBody, const float minYOverlap)
{
	Vector2 position = mOwner->GetPosition();
	position.y -= minYOverlap;
	mOwner->SetPosition(position);

	Vector2 velocity = rigidBody->GetVelocity();
	velocity.y = 0.0f;
	rigidBody->SetVelocity(velocity);

	if (minYOverlap > 0.0f) mOwner->SetOnGround();
}

void AABBColliderComponent::DebugDraw(class Renderer *renderer)
{
	renderer->DrawRect(mOwner->GetPosition() + mOffset, Vector2((float) mWidth, (float) mHeight), mOwner->GetRotation(),
					   Color::Green, mOwner->GetGame()->GetCameraPos(), RendererMode::LINES);
}

void AABBColliderComponent::SetSize(int width, int height)
{
	mWidth = width;
	mHeight = height;
}

void AABBColliderComponent::SetOffset(Vector2 offset)
{
	mOffset = offset;
}