#include "WinScreen.h"
#include "../../Game.h"

WinScreen::WinScreen(class Game* game, const std::string& fontName)
    :UIScreen(game, fontName)
{
    const float SCALE = 0.7f;

    // Stop all sounds and play victory music
    game->GetAudio()->StopAllSounds();
    game->GetAudio()->PlaySound("m02_victory.mp3", true, 0.5f);

    AddImage("../Assets/HUD/Background/WinBackground.png", Vector2::Zero, 1.0f, 0.0f, -1);

    AddText("YOU WIN!", Vector2(0.0f, -150.0f), SCALE, 0.0f, 60);
    
    AddButton("BACK TO MENU", [this, game]() {
        this->Close();
        game->ResetGame();
    }, Vector2(0.0f, +100.0f), SCALE);

    // Setup UI Screen initial state
    mSelectedButtonIndex = 0;
    mButtons[0]->SetHighlighted(true); // new game

    // Customize buttons
    for (auto button : mButtons) {
        button->SetTextColor(Vector3::One);
        button->SetTextHighlightColor(Vector3(0.8f, 0.0f, 0.8f)); // violet
        button->SetBackgroundColor(Vector4::Zero); // white
    }

    // Customize text
    for (auto text : mTexts) {
        text->SetTextColor(Vector3::One);
        text->SetBackgroundColor(Vector4::Zero); // transparent
    }  
}

void WinScreen::HandleKeyPress(int key)
{
    if (mButtons.empty())
        return;

    switch (key) {
    case SDLK_RETURN:
    case SDLK_KP_ENTER:
        mButtons[mSelectedButtonIndex]->OnClick();
        break;

    default:
        break;
    }
}