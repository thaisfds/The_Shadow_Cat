#pragma once
#include "../../Math.h" // For Vector2
#include "RigidBodyComponent.h"
#include "ColliderComponent.h"
#include <vector>

class Collider
{
public:
	virtual ~Collider() = default;

	ColliderComponent* GetComponent() const { return mComponent; }
	void SetComponent(ColliderComponent* component) { mComponent = component; }

	bool CheckCollision(const Collider* other);
	std::vector<ColliderComponent*> GetOverlappingColliders();

	virtual bool CheckCollisionAt(Vector2* newPosition, const Collider* other) const = 0;
	virtual std::vector<ColliderComponent*> GetOverlappingCollidersAt(Vector2* newPosition) const = 0;
	virtual void SolveCollisions(const RigidBodyComponent* rigidBody) = 0;
	virtual void DebugDraw(class Renderer* renderer) = 0;

protected:
	ColliderComponent *mComponent;
};

class AABBCollider : public Collider
{
public:
	AABBCollider(float width, float height) : mHalfDimensions(width / 2.0f, height / 2.0f) {}

	bool CheckCollisionAt(Vector2* newPosition, const Collider* other) const override;
	std::vector<ColliderComponent*> GetOverlappingCollidersAt(Vector2* newPosition) const override;
	void SolveCollisions(const RigidBodyComponent* rigidBody) override;
	void DebugDraw(class Renderer* renderer) override;

	void SetSize(Vector2 size) { mHalfDimensions = size * 0.5f; }

	Vector2 GetMinAt(Vector2 position) const;
	Vector2 GetMaxAt(Vector2 position) const;

	Vector2 GetMin() const { return GetMinAt(mComponent->GetPosition()); }
	Vector2 GetMax() const { return GetMaxAt(mComponent->GetPosition()); }

	Vector2 GetHalfDimensions() const { return mHalfDimensions; }

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

	bool CheckCollisionAt(Vector2* newPosition, const Collider* other) const override;
	std::vector<ColliderComponent*> GetOverlappingCollidersAt(Vector2* newPosition) const override;
	void SolveCollisions(const RigidBodyComponent* rigidBody) override {}
	void DebugDraw(class Renderer* renderer) override;

	float GetRadius() const { return mRadius; }
	void SetRadius(float radius) { mRadius = radius; }

private:
	float mRadius;
};

class PolygonCollider : public Collider
{
	public:
		PolygonCollider(const std::vector<Vector2>& vertices) : mVertices(vertices), mBaseVertices(vertices), mForward(Vector2(1.0f, 0.0f)) {}

		bool CheckCollisionAt(Vector2* newPosition, const Collider* other) const override;
		std::vector<ColliderComponent*> GetOverlappingCollidersAt(Vector2* newPosition) const override;
		void SolveCollisions(const RigidBodyComponent* rigidBody) override {}
		void DebugDraw(class Renderer* renderer) override;

		void SetForward(Vector2 forward);

		const std::vector<Vector2>& GetVertices() const { return mVertices; }

	private:
		std::vector<Vector2> mVertices;
		std::vector<Vector2> mBaseVertices;
		Vector2 mForward; // Normalized direction the polygon is facing
};