#include "MainMenu.h"
#include "../../Game.h"

MainMenu::MainMenu(class Game* game, const std::string& fontName)
    :UIScreen(game, fontName)
{
    const float SCALE = 0.7f;

    // Using lambda as on click functions
    AddButton("NEW ADVENTURE", [game]() {
        game->SetScene(GameScene::Lobby);
    }, Vector2(0.0f, -10.0f), SCALE);

    AddButton("QUIT", [this, game]() {
        this->Close();
        game->Quit();
    }, Vector2(0.0f, +50.0f), SCALE);

    // Playtest Disclaimer
    AddText("v0.8-playtest", Vector2(540.0f, 328.0f), 0.7f);

    // Setup UI Screen initial state
    mSelectedButtonIndex = 0;
    mButtons[0]->SetHighlighted(true); // new game

    // Customize buttons
    for (auto button : mButtons) {
        button->SetTextColor(Vector3::One);
        button->SetTextHighlightColor(Vector3(0.8f, 0.0f, 0.8f)); // violet
        button->SetBackgroundColor(Vector4::Zero); // transparent
    }
    
    // Customize text
    for (auto text : mTexts) {
        text->SetTextColor(Vector3(0.8f));
        text->SetBackgroundColor(Vector4::Zero); // transparent
    }  
}

void MainMenu::HandleKeyPress(int key)
{
    if (mButtons.empty())
        return;

    switch (key) {
    case SDLK_w:
    case SDLK_UP:
        if (mSelectedButtonIndex == 0) break;

        mButtons[mSelectedButtonIndex]->SetHighlighted(false);
        mSelectedButtonIndex--;
        mButtons[mSelectedButtonIndex]->SetHighlighted(true);
        break;
    
    case SDLK_s:
    case SDLK_DOWN:
        if (mSelectedButtonIndex == (int) mButtons.size() - 1) break;

        mButtons[mSelectedButtonIndex]->SetHighlighted(false);
        mSelectedButtonIndex++;
        mButtons[mSelectedButtonIndex]->SetHighlighted(true);
        break;
    
    case SDLK_e:
    case SDLK_RETURN:
    case SDLK_KP_ENTER:
        mButtons[mSelectedButtonIndex]->OnClick();
        break;

    default:
        break;
    }
}