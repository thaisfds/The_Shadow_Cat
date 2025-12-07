#pragma once
#include "Actor.h"

/**
 * LevelPortal - Red carpet that appears when boss is defeated
 * 
 * Purely visual indicator at the end of each level.
 * The carpet appears when all bosses are defeated (or in debug mode).
 * Level transition still uses grid-based detection (player walks to last row).
 */
class LevelPortal : public Actor
{
public:
    LevelPortal(class Game* game);
    
    // Control portal visibility
    void Activate();
    void Deactivate();
    bool IsActive() const { return mIsActive; }
    
private:
    class AnimatorComponent* mAnimatorComponent;
    bool mIsActive;
};
