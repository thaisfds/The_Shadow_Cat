#include "LevelPortal.h"
#include "../Game.h"
#include "../GameConstants.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include <SDL.h>

LevelPortal::LevelPortal(Game* game)
    : Actor(game)
    , mIsActive(false)
    , mAnimatorComponent(nullptr)
{
    // Portal is purely logical - carpet tile from CSV shows when portal is active
    // No visual component needed, the tile 9 in the level handles the visual
    Deactivate();
    SDL_Log("[PORTAL] Created - purely logical (visual is from level tile)");
}

void LevelPortal::Activate()
{
    mIsActive = true;
    SDL_Log("[PORTAL] ACTIVATED - carpet visible (from level tile)");
}

void LevelPortal::Deactivate()
{
    mIsActive = false;
    SDL_Log("[PORTAL] DEACTIVATED");
}
