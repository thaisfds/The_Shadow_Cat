#include "Dash.h"
#include "../../Actors/Characters/Character.h"
#include "../../Components/Physics/RigidBodyComponent.h"
#include "../../Game.h"
#include "../Drawing/AnimatorComponent.h"
#include "../Physics/ColliderComponent.h"

Dash::Dash(Actor* owner, int updateOrder)
	: SkillBase(owner, updateOrder)
{
	mName = "Dash";
	mDescription = "Quickly dash in a direction to evade attacks.";
	mCooldown = 4.0f;
	mCurrentCooldown = 0.0f;
	mIsDashing = false;

	mDashDuration = 0.5f;
	mDashSpeed = 400.0f;

}

void Dash::Update(float deltaTime)
{
	SkillBase::Update(deltaTime);

	if (!mIsDashing) return;
	
	mDashTimer += deltaTime;
	
	if (mDashDuration <= mDashTimer + mCharacter->GetComponent<AnimatorComponent>()->GetAnimationDuration("DashEnd"))
		mCharacter->GetComponent<AnimatorComponent>()->PlayAnimationOnce("DashEnd", false);

	Vector2 dashVelocity = mDashDirection * mDashSpeed;
	mCharacter->GetComponent<RigidBodyComponent>()->SetVelocity(dashVelocity);

	if (mDashTimer >= mDashDuration) EndDash();
}

void Dash::Execute(Vector2 targetPosition)
{
	mCharacter->SetAnimationLock(true);
	AnimatorComponent* animator = mCharacter->GetComponent<AnimatorComponent>();
	animator->LoopAnimation("DashMid");
	animator->PlayAnimationOnce("DashBegin");	
	SDL_Log("Dash executed");

	Vector2 mouseWorldPos = mCharacter->GetGame()->GetMouseWorldPosition();
	mDashDirection = (mouseWorldPos - mCharacter->GetPosition());
	mDashDirection.Normalize();

	if (mDashDirection.x > 0.0f)
        mCharacter->SetScale(Vector2(1.0f, mCharacter->GetScale().y));
    else if (mDashDirection.x < 0.0f)
        mCharacter->SetScale(Vector2(-1.0f, mCharacter->GetScale().y));

	mCharacter->SetMovementLock(true);
	mIsDashing = true;
	mDashTimer = 0.0f;

	CollisionFilter filter = mCharacter->GetComponent<ColliderComponent>()->GetFilter();
	filter.collidesWith = CollisionFilter::RemoveGroups(filter.collidesWith,
		{CollisionGroup::Player, CollisionGroup::Enemy, CollisionGroup::PlayerSkills, CollisionGroup::EnemySkills});
	mCharacter->GetComponent<ColliderComponent>()->SetFilter(filter);

	StartCooldown();
}

void Dash::EndDash()
{
	mCharacter->SetAnimationLock(false);
	mCharacter->SetMovementLock(false);

	mCharacter->GetComponent<ColliderComponent>()->SetFilter(
		Character::GetBasePlayerFilter()
	);

	mIsDashing = false;
}