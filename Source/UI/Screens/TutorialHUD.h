#pragma once

#include <string>

#include "UIScreen.h"

class TutorialHUD : public UIScreen
{
public:
    TutorialHUD(class Game* game, const std::string& fontName);

    bool IsControlVisible() const { return showControls; }

    void ToggleControlVisibility();
    void ShowControls();
    void HideControls();
private:

    // TutorialHUD elements
    bool showControls = false;
};
