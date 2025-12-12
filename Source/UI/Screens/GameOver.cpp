#include "GameOver.h"
#include "../../Game.h"

GameOver::GameOver(class Game* game, const std::string& fontName)
    :UIScreen(game, fontName)
{
    const float SCALE = 0.7f;

    // Stop all sounds and play game over music
    game->GetAudio()->StopAllSounds();
    game->GetAudio()->PlaySound("m03_game_over.mp3", true, 0.5f);

    switch (SceneManager::Instance().GetCurrentScene()) {
    case GameScene::Level1_Boss:
        AddImage("../Assets/HUD/Background/DefeatBackground1.png", Vector2::Zero, 1.0f, 0.0f, -1);
        break;
    case GameScene::Level2_Boss:
        AddImage("../Assets/HUD/Background/DefeatBackground3.png", Vector2::Zero, 1.0f, 0.0f, -1);
        break;
    case GameScene::Level3_Boss:
        AddImage("../Assets/HUD/Background/DefeatBackground2.png", Vector2::Zero, 1.0f, 0.0f, -1);
        break;
    default:
        AddImage("../Assets/HUD/Background/DefeatBackground4.png", Vector2::Zero, 1.0f, 0.0f, -1);
        break;
    }

    AddText("GAME OVER :(", Vector2(0.0f, -150.0f), SCALE, 0.0f, 60);

    AddButton("TRY AGAIN", [this, game]() {
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

void GameOver::HandleKeyPress(int key)
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