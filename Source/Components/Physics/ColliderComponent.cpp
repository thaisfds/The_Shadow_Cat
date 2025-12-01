#include "ColliderComponent.h"
#include "../../Game.h"
#include "CollisionFilter.h"

ColliderComponent::ColliderComponent(class Actor *owner, int dx, int dy, Collider* collider, CollisionFilter filter, bool isStatic, int updateOrder)
	: Component(owner, updateOrder)
	  , mOffset(Vector2((float) dx, (float) dy))
	  , mIsStatic(isStatic)
	  , mFilter(filter)
	  , mCollider(collider)
{
	mCollider->SetComponent(this);
	GetGame()->AddCollider(this);
}

ColliderComponent::~ColliderComponent()
{
	GetGame()->RemoveCollider(this);
}

void ColliderComponent::DetectCollisions(RigidBodyComponent *rigidBody)
{
	if (mIsStatic || !mIsEnabled) return;

	mCollider->SolveCollisions(rigidBody);
}


void ColliderComponent::DebugDraw(class Renderer *renderer)
{
	if (!mIsEnabled) return;
	
	mCollider->DebugDraw(renderer);
}


void ColliderComponent::SetOffset(Vector2 offset)
{
	mOffset = offset;
}

bool ColliderComponent::ShouldCollideWith(ColliderComponent* other) const
{
	if (other == this) return false;
	if (!other->IsEnabled()) return false;
	if (!CollisionFilter::ShouldCollide(mFilter, other->GetFilter())) return false; 

	return true;
}