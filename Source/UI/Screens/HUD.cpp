#include "HUD.h"
#include "../../Game.h"
#include <string>

HUD::HUD(class Game* game, const std::string& fontName)
    :UIScreen(game, fontName)
{
    const Vector2 healthOffset = Vector2(-200.0f, 300.0f);
    const Vector2 scoreOffset = Vector2(-300.0f, 230.0f); // Below health
    const Vector2 scoreValueOffset = Vector2(-230.0f, 230.0f); // Right of Score

    const float scale = 0.75f;

    mHealth3 = AddImage("../Assets/HUD/ShieldBlue.png", healthOffset, scale, 0.0f, 3);
    mHealth2 = AddImage("../Assets/HUD/ShieldOrange.png", healthOffset, scale, 0.0f, 2);
    mHealth1 = AddImage("../Assets/HUD/ShieldRed.png", healthOffset, scale, 0.0f, 1);
    
    
    AddImage("../Assets/HUD/ShieldBar.png", healthOffset, scale, 0.0f, 4);

    AddText("Score: ", scoreOffset, 0.6f);
    mScore = AddText("0", scoreValueOffset, 0.6f);
}

void HUD::SetHealth(int health)
{
    switch (health) {
    case 3:
        mHealth1->SetIsVisible(true);
        mHealth2->SetIsVisible(true);
        mHealth3->SetIsVisible(true);
        break;

    case 2:
        mHealth1->SetIsVisible(true);
        mHealth2->SetIsVisible(true);
        mHealth3->SetIsVisible(false);
        break;

    case 1:
        mHealth1->SetIsVisible(true);
        mHealth2->SetIsVisible(false);
        mHealth3->SetIsVisible(false);
        break;

    case 0:
        mHealth1->SetIsVisible(false);
        mHealth2->SetIsVisible(false);
        mHealth3->SetIsVisible(false);
        break;

    default:
        break;
    }
}

void HUD::SetScore(int score)
{
    mScore->SetText(std::to_string(score));
}
