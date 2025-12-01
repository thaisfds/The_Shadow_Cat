#include "HUD.h"
#include "../../Game.h"
#include <string>

HUD::HUD(class Game* game, const std::string& fontName, int maxHealth)
    :UIScreen(game, fontName),
    mMaxHealth(maxHealth),
    mHealth(maxHealth)
{   
    mMaxHealth = std::max(2, mMaxHealth); // Ensure at least 2
    mHealth = mMaxHealth;

    InitHealthIcons();
}

void HUD::InitHealthIcons() {
    if (mMaxHealth % 2 != 0) {
        mMaxHealth++; // Ensure max health is even for heart icons
        mHealth++;
    }

    // Delete any existing icons
    for (auto img : mFullHeartIcons) delete img;
    for (auto img : mHalfHeartIcons) delete img;
    for (auto img : mEmptyHeartIcons) delete img;

    mFullHeartIcons.clear();
    mHalfHeartIcons.clear();
    mEmptyHeartIcons.clear();

    // Create new icons based on mMaxHealth
    const float SCALE = 1.5f;
    const float SPACING = 35.0f * SCALE;

    for (int i = 0; i < mMaxHealth / 2; ++i) {
        Vector2 offset(-560.0f + i * SPACING, -300.0f);

        UIImage* emptyHeart = AddImage("../Assets/HUD/LifeBar_2.png", offset, SCALE, 0.0f, 1);
        UIImage* halfHeart = AddImage("../Assets/HUD/LifeBar_1.png", offset, SCALE, 0.0f, 2);
        UIImage* fullHeart = AddImage("../Assets/HUD/LifeBar_0.png", offset, SCALE, 0.0f, 3);

        mEmptyHeartIcons.push_back(emptyHeart);
        mHalfHeartIcons.push_back(halfHeart);
        mFullHeartIcons.push_back(fullHeart);
    }

    SetHealth(mHealth);
}

void HUD::SetHealth(int health)
{
    mHealth = std::clamp(health, 0, mMaxHealth);

    // Update heart icons based on current health
    for (int i = 0; i < mMaxHealth / 2; ++i) {
        int heartHealth = (i + 1) * 2;

        if (mHealth >= heartHealth) {
            mFullHeartIcons[i]->SetIsVisible(true);
            mHalfHeartIcons[i]->SetIsVisible(false);
            mEmptyHeartIcons[i]->SetIsVisible(false);
        } else if (mHealth == heartHealth - 1) {
            mFullHeartIcons[i]->SetIsVisible(false);
            mHalfHeartIcons[i]->SetIsVisible(true);
            mEmptyHeartIcons[i]->SetIsVisible(false);
        } else {
            mFullHeartIcons[i]->SetIsVisible(false);
            mHalfHeartIcons[i]->SetIsVisible(false);
            mEmptyHeartIcons[i]->SetIsVisible(true);
        }
    }
}

void HUD::UpdateMaxHealth(int maxHealth, bool fill)
{
    mMaxHealth = maxHealth;

    mMaxHealth = std::max(2, mMaxHealth); // Ensure at least 2
    mHealth = fill ? mMaxHealth : std::min(mHealth, mMaxHealth);

    InitHealthIcons();
}
