#include "SceneManager.h"
#include "Game.h"
#include "LevelManager.h"
#include "GameConstants.h"
#include "UI/Screens/MainMenu.h"
#include "UI/Screens/HUD.h"
#include "UI/Screens/TutorialHUD.h"
#include "UI/Screens/UpgradeHUD.h"

// Singleton instance
SceneManager& SceneManager::Instance()
{
    static SceneManager instance;
    return instance;
}

SceneManager::SceneManager()
    : mGame(nullptr),
      mCurrentScene(GameScene::MainMenu)
{
}

SceneManager::~SceneManager()
{
    Shutdown();
}

void SceneManager::Initialize(Game* game)
{
    mGame = game;
    mCurrentScene = GameScene::MainMenu;
}

void SceneManager::Shutdown()
{
    if (mBackgroundMusic.IsValid() && mGame)
    {
        mGame->GetAudio()->StopSound(mBackgroundMusic);
        mBackgroundMusic.Reset();
    }
    mGame = nullptr;
}

void SceneManager::SetScene(GameScene nextScene)
{
    UnloadScene();
    mCurrentScene = nextScene;

    switch (nextScene)
    {
    case GameScene::MainMenu:
        mBackgroundMusic = mGame->GetAudio()->PlaySound("m01_main_menu.mp3", true, 0.3f);
        new MainMenu(mGame, "../Assets/Fonts/Pixellari.ttf");
        break;

    case GameScene::Lobby:
        mBackgroundMusic = mGame->GetAudio()->PlaySound("m04_tutorial.mp3", true, 0.5f);
        
        if (!mGame->GetTutorialHUD())
            mGame->SetTutorialHUD(new TutorialHUD(mGame, "../Assets/Fonts/Pixellari.ttf"));

        mGame->SetHUD(new HUD(mGame, "../Assets/Fonts/Pixellari.ttf"));
        mGame->SetUpgradeHUD(new UpgradeHUD(mGame, "../Assets/Fonts/Pixellari.ttf"));

        LevelManager::Instance().LoadAndBuildLevel(nextScene);
        break;

    case GameScene::Level1:
        mBackgroundMusic = mGame->GetAudio()->PlaySound("m05_level1.mp3", true, 0.5f);
        LevelManager::Instance().LoadAndBuildLevel(nextScene);
        break;

    case GameScene::Level1_Boss:
        mBackgroundMusic = mGame->GetAudio()->PlaySound("m08_boss1_grass.mp3", true, 0.3f);
        LevelManager::Instance().LoadAndBuildLevel(nextScene);
        break;

    case GameScene::Level2:
        mBackgroundMusic = mGame->GetAudio()->PlaySound("m06_level2.mp3", true, 0.6f);
        LevelManager::Instance().LoadAndBuildLevel(nextScene);
        break;

    case GameScene::Level2_Boss:
        mBackgroundMusic = mGame->GetAudio()->PlaySound("m09_boss2_bricks.mp3", true);
        LevelManager::Instance().LoadAndBuildLevel(nextScene);
        break;

    case GameScene::Level3:
        mBackgroundMusic = mGame->GetAudio()->PlaySound("m07_level3.mp3", true);
        LevelManager::Instance().LoadAndBuildLevel(nextScene);
        break;

    case GameScene::Level3_Boss:
        mBackgroundMusic = mGame->GetAudio()->PlaySound("m10_boss3_stone.mp3", true);
        LevelManager::Instance().LoadAndBuildLevel(nextScene);
        break;
    }

    OnSceneChanged.Invoke(mCurrentScene);
}

void SceneManager::UnloadScene()
{
    if (mBackgroundMusic.IsValid())
    {
        mGame->GetAudio()->StopSound(mBackgroundMusic);
        mBackgroundMusic.Reset();
    }

    LevelManager::Instance().UnloadLevel();

    // Delete UI screens (except persistent HUDs)
    auto& uiStack = mGame->GetUIStack();
    auto tutorialHUD = mGame->GetTutorialHUD();
    auto hud = mGame->GetHUD();
    auto upgradeHUD = mGame->GetUpgradeHUD();

    for (auto ui : uiStack)
    {
        if (ui == hud || ui == tutorialHUD || ui == upgradeHUD)
            continue;
        delete ui;
    }
    uiStack.clear();
    
    if (tutorialHUD)
        uiStack.push_back(tutorialHUD);
    if (hud)
        uiStack.push_back(hud);
    if (upgradeHUD)
        uiStack.push_back(upgradeHUD);
}

GroundType SceneManager::GetGroundType() const
{
    switch (mCurrentScene)
    {
    case GameScene::Lobby:
    case GameScene::Level1:
    case GameScene::Level1_Boss:
        return GroundType::Grass;
    case GameScene::Level2:
    case GameScene::Level2_Boss:
        return GroundType::Brick;
    case GameScene::Level3:
    case GameScene::Level3_Boss:
        return GroundType::Stone;
    default:
        return GroundType::Grass;
    }
}

std::string SceneManager::GetBackgroundPath() const
{
    switch (mCurrentScene)
    {
    case GameScene::MainMenu:
        return "../Assets/HUD/Background/MainMenuBackground.png";
    case GameScene::Lobby:
        return "../Assets/Levels/Lobby/LobbyBackground.png";
    case GameScene::Level1:
        return "../Assets/Levels/Level1/Level1Background.png";
    case GameScene::Level1_Boss:
        return "../Assets/Levels/Level1_Boss/Level1_Boss_Background.png";
    case GameScene::Level2:
        return "../Assets/Levels/Level2/Level2Background.png";
    case GameScene::Level2_Boss:
        return "../Assets/Levels/Level2_Boss/Level2_Boss_Background.png";
    case GameScene::Level3:
        return "../Assets/Levels/Level3/Level3Background.png";
    case GameScene::Level3_Boss:
        return "../Assets/Levels/Level3_Boss/Level3_Boss_Background.png";
    default:
        return "../Assets/Levels/Lobby/LobbyBackground.png";
    }
}
