#pragma once
#include "../Component.h"
#include "../../Math.h"
#include "CollisionFilter.h"
#include "RigidBodyComponent.h"
#include <vector>
#include <set>

class AABBColliderComponent : public Component
{
public:
	AABBColliderComponent(class Actor *owner, int dx, int dy, int w, int h, CollisionFilter filter, bool isStatic = false, int updateOrder = 10);

	~AABBColliderComponent() override;

	bool Intersect(const AABBColliderComponent &b) const;

	float DetectHorizontalCollision(RigidBodyComponent *rigidBody);
	float DetectVertialCollision(RigidBodyComponent *rigidBody);

	Vector2 GetMin() const;

	Vector2 GetMax() const;

	CollisionFilter GetFilter() const { return mFilter; }
	void SetFilter(CollisionFilter filter) { mFilter = filter; }

	// Drawing for debug purposes
	void DebugDraw(class Renderer *renderer) override;

	void SetSize(int width, int height);

	void SetOffset(Vector2 offset);

private:
	float GetMinVerticalOverlap(AABBColliderComponent *b) const;
	float GetMinHorizontalOverlap(AABBColliderComponent *b) const;

	void ResolveHorizontalCollisions(RigidBodyComponent *rigidBody, const float minOverlap);
	void ResolveVerticalCollisions(RigidBodyComponent *rigidBody, const float minOverlap);

	bool ShouldCollideWith(AABBColliderComponent* other) const;

	Vector2 mOffset;
	int mWidth;
	int mHeight;
	bool mIsStatic;

	CollisionFilter mFilter;
};