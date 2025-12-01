#include "TutorialHUD.h"
#include "../../Game.h"
#include <string>

TutorialHUD::TutorialHUD(class Game* game, const std::string& fontName)
    :UIScreen(game, fontName)
{
    // WASD
    AddImage("../Assets/HUD/wasd.png", Vector2(-100.0f, 0.0f), 0.15f, 0.0f, 1);
    
    AddText("Move", Vector2(-100.0f, 40.0f), 0.4f);

    // Skills
    // AddImage("../Assets/HUD/q.png", Vector2(100.0f, -20.0f), 0.15f, 0.0f, 1);
    // AddImage("../Assets/HUD/e.png", Vector2(100.0f, 10.0f), 0.15f, 0.0f, 1);
    // AddImage("../Assets/HUD/r.png", Vector2(100.0f, 40.0f), 0.15f, 0.0f, 1);

    AddText("Left Mouse: Basic Attack", Vector2(200.0f, -20.0f), 0.4f);
    AddText("Right Mouse: Claw Attack", Vector2(200.0f, 10.0f), 0.4f);
    AddText("E: Furball", Vector2(160.0f, 40.0f), 0.4f);
    AddText("Q: Stomp", Vector2(160.0f, 70.0f), 0.4f);
    AddText("Shift: Dash", Vector2(160.0f, 100.0f), 0.4f);

    // Toggle visibility instruction
    AddText("Press H to hide/show controls guide", Vector2(0.0f, 200.0f), 0.4f);

    for (auto &txt : mTexts) {
        txt->SetTextColor(Vector3::Zero); // Black
        txt->SetBackgroundColor(Vector4::Zero); // Transparent
    }
}

void TutorialHUD::ToggleControlVisibility()
{
    showControls = !showControls;

    for (auto &img : mImages)
        img->SetIsVisible(showControls);

    for (auto &txt : mTexts)
        txt->SetIsVisible(showControls);
}