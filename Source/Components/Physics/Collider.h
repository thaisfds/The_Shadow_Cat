#pragma once
#include "../../Math.h" // For Vector2
#include "RigidBodyComponent.h"

class ColliderComponent;

class Collider
{
public:
	virtual ~Collider() = default;

	ColliderComponent* GetComponent() const { return mComponent; }
	void SetComponent(ColliderComponent* component) { mComponent = component; }

	virtual bool CheckCollision(const Collider* other) const = 0;
	virtual void SolveCollisions(const RigidBodyComponent* rigidBody) = 0;
	virtual void DebugDraw(class Renderer* renderer) = 0;

protected:
	ColliderComponent *mComponent;
};

class AABBCollider : public Collider
{
public:
	AABBCollider(float width, float height) : mHalfDimensions(width / 2.0f, height / 2.0f) {}

	bool CheckCollision(const Collider* other) const override;
	void SolveCollisions(const RigidBodyComponent* rigidBody) override;
	void DebugDraw(class Renderer* renderer) override;

	Vector2 GetMin() const;
	Vector2 GetMax() const;

	Vector2 GetHalfDimensions() const { return mHalfDimensions; }

	void DebugDraw(class Renderer* renderer) override;

private:
	float GetMinVerticalOverlap(AABBCollider *b) const;
	float GetMinHorizontalOverlap(AABBCollider *b) const;

	void ResolveHorizontalCollisions(RigidBodyComponent *rigidBody, const float minOverlap);
	void ResolveVerticalCollisions(RigidBodyComponent *rigidBody, const float minOverlap);

	Vector2 mHalfDimensions;
};

class CircleCollider : public Collider
{
public:
	CircleCollider(float radius) : mRadius(radius) {}

	bool CheckCollision(const Collider* other) const override;

	float GetRadius() const { return mRadius; }

private:
	float mRadius;
};