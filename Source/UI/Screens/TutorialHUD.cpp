#include "TutorialHUD.h"
#include "../../Game.h"
#include "../../SceneManager.h"
#include <string>

TutorialHUD::TutorialHUD(class Game* game, const std::string& fontName)
    :UIScreen(game, fontName)
{   
    AddImage("../Assets/HUD/Menu/Scroll1.png", Vector2(0.0f, -178.0f), 13.0f, 0.0f, 1);

    // WASD
    AddImage("../Assets/HUD/Menu/KeyboardW.png", Vector2(-220.0f, -240.0f), 0.8f, 0.0f);
    AddImage("../Assets/HUD/Menu/KeyboardA.png", Vector2(-195.0f, -240.0f), 0.8f, 0.0f);
    AddImage("../Assets/HUD/Menu/KeyboardS.png", Vector2(-170.0f, -240.0f), 0.8f, 0.0f);
    AddImage("../Assets/HUD/Menu/KeyboardD.png", Vector2(-145.0f, -240.0f), 0.8f, 0.0f);

    // Skills
    AddImage("../Assets/HUD/Menu/KeyboardLMB.png", Vector2(-220.0f, -200.0f), 0.8f, 0.0f);
    AddImage("../Assets/HUD/Menu/KeyboardRMB.png", Vector2(-100.0f, -200.0f), 0.8f, 0.0f);

    AddImage("../Assets/HUD/Menu/KeyboardQ.png", Vector2(40.0f, -240.0f), 0.8f, 0.0f);
    AddImage("../Assets/HUD/Menu/KeyboardE.png", Vector2(40.0f, -215.0f), 0.8f, 0.0f);
    AddImage("../Assets/HUD/Menu/KeyboardSHIFT.png", Vector2(40.0f, -190.0f), 1.0f, 0.0f);

    // Mouse aim
    AddImage("../Assets/Icons/Cursor.png", Vector2(255.0f, -240.0f), 0.6f, 0.0f);
    AddImage("../Assets/Icons/Cursor.png", Vector2(255.0f, -215.0f), 0.6f, 0.0f);
    AddImage("../Assets/Icons/Cursor.png", Vector2(255.0f, -190.0f), 0.6f, 0.0f);

    AddText("Move", Vector2(-110.0f, -240.0f), 0.4f);
    AddText("Basic Attack", Vector2(-165.0f, -200.0f), 0.4f);
    AddText("Claw Attack", Vector2(-45.0f, -200.0f), 0.4f);

    AddText("Stomp - Aim with mouse", Vector2(150.0f, -240.0f), 0.4f);
    AddText("Furball - Aim with mouse", Vector2(150.0f, -215.0f), 0.4f);
    AddText("Dash - Aim with mouse", Vector2(150.0f, -190.0f), 0.4f);

    // Toggle visibility instruction
    AddImage("../Assets/HUD/Menu/KeyboardH.png", Vector2(-110.0f, -165.0f), 0.8f, 0.0f);

    AddText("Hide/Show controls guide", Vector2(0.0f, -165.0f), 0.4f);

    for (auto &txt : mTexts) {
        txt->SetTextColor(Vector3::Zero); // Black
        txt->SetBackgroundColor(Vector4::Zero); // Transparent
    }

    SceneManager::Instance().OnSceneChanged.Subscribe([this](GameScene scene) {
        OnSceneChanged(scene);
    });
}

void TutorialHUD::SetControlVisibility(bool visible)
{
    showControls = visible;

    for (auto &img : mImages) img->SetIsVisible(visible);
    for (auto &txt : mTexts) txt->SetIsVisible(visible);
}

void TutorialHUD::OnKeyPress(int key)
{
    if (key != SDLK_h) return;

    SetControlVisibility(!showControls);
}

void TutorialHUD::OnSceneChanged(GameScene newScene)
{
    SetControlVisibility(newScene == GameScene::Lobby);
}