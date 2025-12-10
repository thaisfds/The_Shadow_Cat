#include "HUD.h"
#include "../../Game.h"
#include "../../GameConstants.h"
#include "../../Actors/Characters/ShadowCat.h"
#include <string>

HUD::HUD(class Game* game, const std::string& fontName, int maxHealth)
    :UIScreen(game, fontName),
    mMaxHealth(maxHealth),
    mHealth(maxHealth)
{   
    // Main health icons, top left
    mMaxHealth = std::max(2, mMaxHealth); // Ensure at least 2
    mHealth = mMaxHealth;

    InitHealthIcons();

    // Cursor
    mCursorImage = AddImage("../Assets/HUD/ShadowCat/Cursor.png", Vector2(0.0f, 0.0f), 1.0f, 0.0f, 20000);

    // Enemy counter top right
    AddText("Enemies Left:", Vector2(500.0f, -300.0f), 0.7f);
    mEnemiesLeftCount = AddText("0", Vector2(600.0f, -300.0f), 0.7f);

    mAreaClearTxt = AddText("            Area clear!\nProceed to the red carpet!", Vector2(630.0f, -220.0f), 0.5f);
    mAreaClearTxt->SetIsVisible(false);

    for (auto &txt : mTexts) {
        txt->SetTextColor(Vector3::One); // White
        txt->SetBackgroundColor(Vector4::Zero); // Transparent
    }
}

void HUD::Update(float deltaTime)
{
    // Update cursor pos  ------------------- //
    Vector2 mouseAbsPos = mGame->GetMouseAbsolutePosition();
    Vector2 mouseRelPos = mGame->GetMouseWorldPosition();
    Vector2 playerPos = mGame->GetPlayer() ? mGame->GetPlayer()->GetPosition() : Vector2::Zero;

    // Rotate cursor based on relative deviation from player
    Vector2 dir = mouseRelPos - playerPos;

    float angle = Math::Atan2(dir.y, dir.x) + Math::PiOver2;

    mCursorImage->SetAngle(angle);
    mCursorImage->SetAbsolutePos(mouseAbsPos);

    // Update enemies left  ------------------- //
    int enemiesLeft = mGame->CountAliveEnemies();
    mEnemiesLeftCount->SetText(std::to_string(enemiesLeft));

    // Update health ------------------- //
    if (!mGame->GetPlayer()) return;

    // If new max hp update otherwise just update health
    if (mGame->GetPlayer()->GetMaxHP() != mMaxHealth) {
        UpdateMaxHealth(mGame->GetPlayer()->GetMaxHP() / 10, true);
    }

    SetHealth(mGame->GetPlayer()->GetHP() / 10);
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
    const float SCALE = 1.0f;
    const float SPACING = 35.0f * SCALE;
    const float VERTICAL_SPACING = 30.0f;

    // Draw two equal rows if more than 10 health
    int rows = (mMaxHealth > 10) ? 2 : 1;

    for (int i = 0; i < mMaxHealth / 2; ++i) {
        int row = i > mMaxHealth / 4 - 1 ? 1 : 0;
        Vector2 offset(-580.0f + (i - row * (mMaxHealth / 4)) * SPACING, +300.0f - row * VERTICAL_SPACING);

        UIImage* emptyHeart = AddImage("../Assets/HUD/ShadowCat/LifeBar_2.png", offset, SCALE, 0.0f, 1);
        UIImage* halfHeart = AddImage("../Assets/HUD/ShadowCat/LifeBar_1.png", offset, SCALE, 0.0f, 2);
        UIImage* fullHeart = AddImage("../Assets/HUD/ShadowCat/LifeBar_0.png", offset, SCALE, 0.0f, 3);

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
