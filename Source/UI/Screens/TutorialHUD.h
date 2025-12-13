#pragma once

#include <string>

#include "UIScreen.h"

class TutorialHUD : public UIScreen
{
public:
    TutorialHUD(class Game* game, const std::string& fontName);

    bool IsControlVisible() const { return showControls; }

    void SetControlVisibility(bool visible);

    void OnKeyPress(int key) override;

    void OnSceneChanged(GameScene newScene);

private:

    // TutorialHUD elements
    bool showControls = false;
};
