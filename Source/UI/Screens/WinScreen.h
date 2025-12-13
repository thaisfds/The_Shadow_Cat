#pragma once

#include "UIScreen.h"

class WinScreen : public UIScreen
{
public:
    WinScreen(class Game* game, const std::string& fontName);

    void OnActiveKeyPress(int key) override;
};