#pragma once
#include "../Component.h"
#include "../../Math.h"
#include "CollisionFilter.h"
#include "RigidBodyComponent.h"
#include "../../Actors/Actor.h"

class Collider;

class ColliderComponent : public Component
{
public:
	ColliderComponent(class Actor *owner, int dx, int dy, Collider* collider, CollisionFilter filter, bool isStatic = false, int updateOrder = 10);

	~ColliderComponent() override;

	bool Intersect(const ColliderComponent &b) const;

	void DetectCollisions(RigidBodyComponent *rigidBody);
	
	Vector2 GetPosition() const { return mOwner->GetPosition() + mOffset; }
	Collider* GetCollider() const { return mCollider; }

	CollisionFilter GetFilter() const { return mFilter; }
	void SetFilter(CollisionFilter filter) { mFilter = filter; }

	void SetCollider(Collider* collider);
    
	void SetSize(int width, int height);
	void SetOffset(Vector2 offset);
    
	void DebugDraw(class Renderer *renderer) override;
	void SetDebugDrawIfDisabled(bool draw) { mDebugDrawIfDisabled = draw; }

private:

	bool ShouldCollideWith(ColliderComponent* other) const;

	Collider *mCollider;
	Vector2 mOffset;
	bool mIsStatic = false;
	bool mDebugDrawIfDisabled = false;

	CollisionFilter mFilter;
};