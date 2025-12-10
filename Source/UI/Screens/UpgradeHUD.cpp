#include "UpgradeHUD.h"
#include "../../Game.h"
#include "../../GameConstants.h"
#include "../../Actors/Characters/ShadowCat.h"
#include <string>

UpgradeHUD::UpgradeHUD(class Game* game, const std::string& fontName)
    :UIScreen(game, fontName)
{   
   
}

void UpgradeHUD::Clear() {
    // Delete all previous elements
    for (auto img : mUpgradeCardBorders) delete img;
    for (auto img : mUpgradeCardSelectedBorders) delete img;
    for (auto img : mUpgradeCardIcons) delete img;
    for (auto txt : mUpgradeTexts) delete txt;

    mUpgradeCardBorders.clear();
    mUpgradeCardSelectedBorders.clear();
    mUpgradeCardIcons.clear();
    mUpgradeTexts.clear();

    if (mBackImage) {
        delete mBackImage;
        mBackImage = nullptr;
    }

    if (mInfoText) {
        delete mInfoText;
        mInfoText = nullptr;
    }

    mSelectedButtonIndex = 0;
}

void UpgradeHUD::InitCards() {
    Clear();
    if (mGame->GetPlayer() == nullptr) return;

    // Opacity background
    mBackImage = AddImage("../Assets/HUD/Background/UpgradeBackground.png", Vector2::Zero, 1.0f, 0.0f, -1);

    mInfoText = AddText("Select an Upgrade", Vector2(0.0f, -250.0f), 0.8f);
    mInfoText->SetTextColor(Vector3::One); // White
    mInfoText->SetBackgroundColor(Vector4::Zero); // Transparent

    std::vector<UpgradeInfo> upgrades = mGame->GetPlayer()->GetRandomUpgrades();
    // Copy for reference
    mCurrentUpgradeInfo = upgrades;

    // Draw cards
    for (size_t i = 0; i < upgrades.size(); ++i) {
        Vector2 cardPos(CARD_INIT_X + i * CARD_SPACING, CARD_Y);

        UIImage* border = AddImage("../Assets/HUD/Cards/Cards9.png", cardPos, CARD_SCALE, 0.0f, 1);
        UIImage* selectedBorder = AddImage("../Assets/HUD/Cards/Cards10.png", cardPos, CARD_SCALE, 0.0f, 3);
        
        std::string descText = upgrades[i].name + "\n\n"+ upgrades[i].skill->GetName() + "\n\n\n\n\n\n\n\nLevel: " + std::to_string(upgrades[i].currentLevel) + (upgrades[i].maxLevel >= 0 ? ("/" + std::to_string(upgrades[i].maxLevel)) : "") + (upgrades[i].value > 0 ? "\n+" : "\n") + std::to_string(upgrades[i].value);
        // Cut 2 decimnal places for float values
        size_t dotPos = descText.find('.');
        if (dotPos != std::string::npos && dotPos + 3 < descText.length()) {
            descText = descText.substr(0, dotPos + 3);
        }
        
        UIText* desc = AddText(descText, cardPos + Vector2(170.0f, TEXT_OFFSET_Y), 0.4f);

        UIImage* icon = AddImage(upgrades[i].skill->GetIconPath(), cardPos + Vector2(0.0f, -10.0f), CARD_SCALE * 0.5f, 0.0f, 2);
       
        mUpgradeCardBorders.push_back(border);
        mUpgradeCardSelectedBorders.push_back(selectedBorder);
        mUpgradeTexts.push_back(desc);
        mUpgradeCardIcons.push_back(icon);
    }

    // Format
    for (auto txt : mUpgradeTexts) {
        txt->SetTextColor(Vector3::One); // White
        txt->SetBackgroundColor(Vector4::Zero); // Transparent
    }

    // Init selected borders
    for (size_t i = 0; i < mUpgradeCardSelectedBorders.size(); ++i) {
        if (i == (size_t)mSelectedButtonIndex)
            mUpgradeCardSelectedBorders[i]->SetIsVisible(true);
        else
            mUpgradeCardSelectedBorders[i]->SetIsVisible(false);
    }
}

void UpgradeHUD::UpdateSelectedCard(int indexChange)
{
    if (mUpgradeCardSelectedBorders.empty()) return;

    mUpgradeCardSelectedBorders[mSelectedButtonIndex]->SetIsVisible(false);
    mSelectedButtonIndex += indexChange;
    int upgradeCount = static_cast<int>(mUpgradeCardSelectedBorders.size());
    if (mSelectedButtonIndex < 0) mSelectedButtonIndex = upgradeCount - 1;
    if (mSelectedButtonIndex >= (int)upgradeCount) mSelectedButtonIndex = 0;
    mUpgradeCardSelectedBorders[mSelectedButtonIndex]->SetIsVisible(true);
}

void UpgradeHUD::Update(float deltaTime)
{   
    // Nothing to show
    if (mGame->GetPlayer() == nullptr) return;
    if (mGame->GetPlayer()->GetUpgradePoints() < 1) return;

    // Already paused
    if (mGame->IsPaused() || mBackImage != nullptr) return;

    InitCards();

    // Pause Game
    mGame->PauseGame();
}


void UpgradeHUD::HandleKeyPress(int key)
{
    if (mUpgradeCardSelectedBorders.empty())
        return;

    switch (key) {
    case SDLK_a:
    case SDLK_LEFT:
        UpdateSelectedCard(-1);
        break;
    
    case SDLK_d:
    case SDLK_RIGHT:
        UpdateSelectedCard(1);
        break;
    
    case SDLK_e:
    case SDLK_SPACE:
    case SDLK_RETURN:
    case SDLK_KP_ENTER:
        mGame->GetPlayer()->SpendUpgradePoint(mCurrentUpgradeInfo[mSelectedButtonIndex]);
        mGame->ResumeGame();

        this->Clear();
        break;

    default:
        break;
    }
}