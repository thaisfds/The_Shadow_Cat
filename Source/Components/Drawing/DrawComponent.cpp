#include "DrawComponent.h"
#include "../../Game.h"
#include "../../LevelManager.h"
#include "../../Actors/Actor.h"

DrawComponent::DrawComponent(class Actor *owner, int drawOrder)
    : Component(owner), mDrawOrder(drawOrder), mIsVisible(true), mColor(Vector3::One)
{
    LevelManager::Instance().AddDrawable(this);
}

DrawComponent::~DrawComponent()
{
    LevelManager::Instance().RemoveDrawable(this);
}

void DrawComponent::Draw(Renderer *renderer)
{
}
