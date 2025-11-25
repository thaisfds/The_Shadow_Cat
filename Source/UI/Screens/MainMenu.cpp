#include "MainMenu.h"
#include "../../Game.h"

MainMenu::MainMenu(class Game* game, const std::string& fontName)
    :UIScreen(game, fontName)
{
    const float SCALE = 0.6f;

    AddImage("../Assets/HUD/ShieldBar.png", Vector2(0.0f, 100.0f), SCALE);

    // Using lambda as on click functions
    AddButton("NEW GAME", [game]() {
        game->SetScene(GameScene::Level1);
    }, Vector2(0.0f, -50.0f), SCALE);

    AddButton("QUIT", [this, game]() {
        this->Close();
        game->Quit();
    }, Vector2(0.0f, +50.0f), SCALE);

    // Setup UI Screen initial state
    mSelectedButtonIndex = 0;
    mButtons[0]->SetHighlighted(true); // new game

    // Customize buttons
    for (auto button : mButtons) {
        button->SetTextColor(Vector3(1.0f, 1.0f, 1.0f));
        button->SetBackgroundColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
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