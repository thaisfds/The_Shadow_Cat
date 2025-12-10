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
    // Pause stuff, persistent
    mPauseText = AddText("Game Paused", Vector2(0.0f, 0.0f), 1.0f, 0.0f, 60, 1024, 10000);
    mPauseFade = AddImage("../Assets/HUD/Background/UpgradeBackground.png", Vector2::Zero, 1.0f, 0.0f, -1);
    
    mPauseFade->SetIsVisible(false);
    mPauseText->SetIsVisible(false);

    // Main health icons, top left
    mMaxHealth = std::max(2, mMaxHealth); // Ensure at least 2
    mHealth = mMaxHealth;

    InitHealthIcons();

    // Cursor
    mCursorImage = AddImage("../Assets/Icons/Cursor.png", Vector2(0.0f, 0.0f), 1.0f, 0.0f, 20000);

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

void HUD::InitSkillIcons() {
    for (auto img : mSkillBorders) delete img;
    for (auto img : mSkillIcons) delete img;
    for (auto txt : mSkillCDText) delete txt;
    for (auto img : mSkillHints) delete img;
    mSkillBorders.clear();
    mSkillIcons.clear();
    mSkillCDText.clear();
    mSkillHints.clear();

    // Create skill icons
    const float SCALE = 1.1f;
    const float SPACING = 60.0f * SCALE;
    Vector2 startOffset(300.0f, 300.0f);

    ShadowCat* player = mGame->GetPlayer();
    if (!player) return;

    auto skills = player->GetSkills();

    // Change order to match input layout
    auto skillsCopy = skills;

    skills[0] = skillsCopy[4]; // LMB
    skills[1] = skillsCopy[2]; // RMB
    skills[2] = skillsCopy[0]; // Q
    skills[3] = skillsCopy[1]; // E
    skills[4] = skillsCopy[3]; // SHIFT

    for (size_t i = 0; i < skills.size(); ++i) {
        Vector2 offset = startOffset + Vector2(i * SPACING, 0.0f);

        UIImage* border = AddImage("../Assets/Icons/ItemSlot.png", offset, SCALE * 1.4f, 0.0f, 1);
        UIImage* icon = AddImage(skills[i]->GetIconPath(), offset, SCALE, 0.0f, 2);

        UIText* cdText = AddText("", offset + Vector2(0.0f, 0.0f), 0.6f);

        mSkillBorders.push_back(border);
        mSkillIcons.push_back(icon);
        mSkillCDText.push_back(cdText);

        // Update cooldown text
        float cdRemaining = skills[i]->GetCooldown();
        if (cdRemaining > 0.0f) {
            cdText->SetText(std::to_string(static_cast<int>(std::ceil(cdRemaining))));
        } else {
            cdText->SetText("");
        }

        // Add keyboard hints
        switch (i) {
            case 0: // LMB
                mSkillHints.push_back(AddImage("../Assets/HUD/Menu/KeyboardLMB.png", offset + Vector2(20.0f, 20.0f), 0.7f, 0.0f));
                break;

            case 1: // RMB
                mSkillHints.push_back(AddImage("../Assets/HUD/Menu/KeyboardRMB.png", offset + Vector2(20.0f, 20.0f), 0.7f, 0.0f));
                break;

            case 2: // Q
                mSkillHints.push_back(AddImage("../Assets/HUD/Menu/KeyboardQ.png", offset + Vector2(20.0f, 20.0f), 0.7f, 0.0f));
                break;

            case 3: // E
                mSkillHints.push_back(AddImage("../Assets/HUD/Menu/KeyboardE.png", offset + Vector2(20.0f, 20.0f), 0.7f, 0.0f));
                break;

            case 4: // SHIFT
                mSkillHints.push_back(AddImage("../Assets/HUD/Menu/KeyboardSHIFT.png", offset + Vector2(20.0f, 20.0f), 0.9f, 0.0f));
                break;

            default:
                break;
        }

    }

    for (auto &txt : mSkillCDText) {
        txt->SetTextColor(Vector3::One); // White
        txt->SetBackgroundColor(Vector4::Zero); // Transparent
    }
}

void HUD::Update(float deltaTime)
{
    // Update skills
    InitSkillIcons();

    // Update cursor pos  ------------------- //
    Vector2 mouseAbsPos = mGame->GetMouseAbsolutePosition();
    Vector2 mouseRelPos = mGame->GetMouseWorldPosition();
    Vector2 playerPos = mGame->GetPlayer() ? mGame->GetPlayer()->GetPosition() : Vector2::Zero;

    // Rotate cursor based on relative deviation from player
    Vector2 dir = mouseRelPos - playerPos;

    float angle = Math::Atan2(dir.y, dir.x) + Math::PiOver2;

    mCursorImage->SetAngle(angle);
    mCursorImage->SetAbsolutePos(mouseAbsPos);

    // Pause handlers
    if (mGame->IsPaused()) {
        mCursorImage->SetIsVisible(false);
    } else {
        mCursorImage->SetIsVisible(true);
    }

    // Pause with no upgrades is normal pause
    if (mGame->IsPaused() && mGame->GetPlayer() && mGame->GetPlayer()->GetUpgradePoints() == 0) {
        mPauseFade->SetIsVisible(true);
        mPauseText->SetIsVisible(true);
    } else {
        mPauseFade->SetIsVisible(false);
        mPauseText->SetIsVisible(false);
    }

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

        UIImage* emptyHeart = AddImage("../Assets/Icons/LifeBar_2.png", offset, SCALE, 0.0f, 1);
        UIImage* halfHeart = AddImage("../Assets/Icons/LifeBar_1.png", offset, SCALE, 0.0f, 2);
        UIImage* fullHeart = AddImage("../Assets/Icons/LifeBar_0.png", offset, SCALE, 0.0f, 3);

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
