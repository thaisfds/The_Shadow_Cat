#pragma once

#include "UIScreen.h"

class GameOver : public UIScreen
{
public:
    GameOver(class Game* game, const std::string& fontName);

    void OnActiveKeyPress(int key) override;
};