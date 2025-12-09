#include "LevelPortal.h"
#include "../Game.h"
#include "../GameConstants.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include <SDL.h>

LevelPortal::LevelPortal(Game* game)
    : Actor(game)
    , mIsActive(false)
{
    // Use Floor animation for the red carpet
    mAnimatorComponent = new AnimatorComponent(this, "Floor", GameConstants::TILE_SIZE, GameConstants::TILE_SIZE);
    mAnimatorComponent->LoopAnimation("Floor5");  // Red carpet tile
    
    // Start invisible
    Deactivate();
    SDL_Log("[PORTAL] Created - initially hidden");
}

void LevelPortal::Activate()
{
    mIsActive = true;
    mAnimatorComponent->SetVisible(true);
    mAnimatorComponent->SetEnabled(true);
    SDL_Log("[PORTAL] ACTIVATED - carpet now visible");
}

void LevelPortal::Deactivate()
{
    mIsActive = false;
    mAnimatorComponent->SetVisible(false);
    mAnimatorComponent->SetEnabled(false);
    SDL_Log("[PORTAL] DEACTIVATED - carpet now hidden");
}
