#pragma once

#include <string>

#include "UIScreen.h"
#include "../../Actors/Characters/ShadowCat.h"

class UpgradeHUD : public UIScreen
{
public:
    UpgradeHUD(class Game* game, const std::string& fontName);

    void Update(float deltaTime) override;
    void HandleKeyPress(int key) override;

private:
    const float CARD_SPACING = 200.0f;
    const float CARD_INIT_X = -400.0f;
    const float CARD_Y = 0.0f;
    const float CARD_SCALE = 6.0f;
    const float TEXT_OFFSET_Y = -5.0f;
    const float BUTTON_OFFSET_Y = -150.0f;

    void Clear();
    void InitCards();
    void UpdateSelectedCard(int indexChange);

    // Back
    UIImage* mBackImage = nullptr;

    // Dynamic upgrade card elements
    std::vector<UIImage*> mUpgradeCardBorders;
    std::vector<UIImage*> mUpgradeCardSelectedBorders;
    std::vector<UIImage*> mUpgradeCardIcons;

    std::vector<UIText*> mUpgradeTexts;

    // Upgrade Info copy
    std::vector<UpgradeInfo> mCurrentUpgradeInfo;
};
