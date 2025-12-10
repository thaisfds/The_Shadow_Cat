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
    UIText* mAreaClearTxt;

    // Cursor
    UIImage* mCursorImage;

    // Boss health bar
    UIImage* mBossHealthBackground;
    UIImage* mBossHealthLeft;
    UIImage* mBossHealthMiddle;
    UIImage* mBossHealthRight;
    std::vector<UIImage*> mBossHealthSegments; // Individual BossLife_3.png segments (40 total, each = 2 HP)
    void InitBossHealthBar();
    void UpdateBossHealth();
};
