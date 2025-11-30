#pragma once
#include "../Component.h"
#include "../../Math.h"
#include "CollisionFilter.h"
#include "RigidBodyComponent.h"
#include "../../Actors/Actor.h"
#include "Collider.h"


class ColliderComponent : public Component
{
public:
	ColliderComponent(class Actor *owner, int dx, int dy, Collider* collider, CollisionFilter filter, bool isStatic = false, int updateOrder = 10);

	~ColliderComponent() override;

	bool Intersect(const ColliderComponent &b) const;

	void DetectCollisions(RigidBodyComponent *rigidBody);
	
	// float DetectHorizontalCollision(RigidBodyComponent *rigidBody);
	// float DetectVertialCollision(RigidBodyComponent *rigidBody);

	// Vector2 GetMin() const;
	// Vector2 GetMax() const;

	Vector2 GetPosition() const { return mOwner->GetPosition() + mOffset; }
	Collider* GetCollider() const { return mCollider; }

	CollisionFilter GetFilter() const { return mFilter; }
	void SetFilter(CollisionFilter filter) { mFilter = filter; }
    
	void SetSize(int width, int height);
	void SetOffset(Vector2 offset);
    
	void DebugDraw(class Renderer *renderer) override;

private:
	// float GetMinVerticalOverlap(ColliderComponent *b) const;
	// float GetMinHorizontalOverlap(ColliderComponent *b) const;

	// void ResolveHorizontalCollisions(RigidBodyComponent *rigidBody, const float minOverlap);
	// void ResolveVerticalCollisions(RigidBodyComponent *rigidBody, const float minOverlap);

	bool ShouldCollideWith(ColliderComponent* other) const;

	Collider *mCollider;
	Vector2 mOffset;
	bool mIsStatic = false;

	CollisionFilter mFilter;
};