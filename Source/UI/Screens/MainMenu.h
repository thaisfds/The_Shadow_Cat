#pragma once

#include "UIScreen.h"

class MainMenu : public UIScreen
{
public:
    MainMenu(class Game* game, const std::string& fontName);

    void OnActiveKeyPress(int key) override;
};