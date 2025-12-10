#include "MainMenu.h"
#include "../../Game.h"
#include "../../GameConstants.h"

MainMenu::MainMenu(class Game* game, const std::string& fontName)
    :UIScreen(game, fontName)
{
    const float SCALE = 0.7f;

    AddImage("../Assets/HUD/Menu/Scroll2.png", Vector2(0.0f, 0.0f), 3.0f, 0.0f, 0);

    // Drop shadow on text
    AddText("    New\nAdventure", Vector2(303.0f, -17.0f), SCALE);
    AddText("Quit", Vector2(3.0f, 43.0f), SCALE);

    // Customize text
    for (auto text : mTexts) {
        text->SetTextColor(Vector3(0.6f));
        text->SetBackgroundColor(Vector4::Zero); // transparent
    }  

    // Using lambda as on click functions
    AddButton("    New\nAdventure", [game]() {
        game->SetScene(GameConstants::DEFAULT_STARTING_SCENE);
    }, Vector2(300.0f, -20.0f), SCALE);

    AddButton("Quit", [this, game]() {
        this->Close();
        game->Quit();
    }, Vector2(0.0f, 40.0f), SCALE);

    // Playtest Disclaimer
    UIText* vtext = AddText("v1.0-release", Vector2(540.0f, 328.0f), 0.7f);
    vtext->SetTextColor(Vector3(0.8f));
    vtext->SetBackgroundColor(Vector4::Zero); // transparent

    // Setup UI Screen initial state
    mSelectedButtonIndex = 0;
    mButtons[0]->SetHighlighted(true); // new game

    // Customize buttons
    for (auto button : mButtons) {
        button->SetTextColor(Vector3::One);
        button->SetTextHighlightColor(Vector3(0.9f, 0.0f, 0.9f)); // violet
        button->SetBackgroundColor(Vector4::Zero); // transparent
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