#pragma once

#include <string>

#include "UIScreen.h"

class HUD : public UIScreen
{
public:
    HUD(class Game* game, const std::string& fontName, int maxHealth = 6);

    void Update(float deltaTime) override;

private:
    // Current health values
    int mHealth;
    int mMaxHealth;

    // Health image handling
    void InitHealthIcons();
    void SetHealth(int health);
    void UpdateMaxHealth(int maxHealth, bool fill = false);
    
    // HUD elements
    std::vector<UIImage*> mFullHeartIcons;
    std::vector<UIImage*> mHalfHeartIcons;
    std::vector<UIImage*> mEmptyHeartIcons;

    // Enemies left
    UIText* mEnemiesLeftCount;

    // Cursor
    UIImage* mCursorImage;
};
