#include "Game.h"
#include "LevelManager.h"
#include "SystemInitializer.h"
#include "SkillFactory.h"
#include "Random.h"
#include "Renderer/Renderer.h"
#include "AudioSystem.h"
#include "GameConstants.h"
#include "UI/Screens/MainMenu.h"
#include "UI/Screens/HUD.h"
#include "UI/Screens/TutorialHUD.h"
#include "UI/Screens/UpgradeHUD.h"
#include "UI/Screens/GameOver.h"
#include "UI/Screens/WinScreen.h"
#include <SDL.h>

#include "Actors/Characters/Boss.h"
#include "Components/Drawing/DrawComponent.h"

Game::Game()
    : mRenderer(nullptr),
      mAudio(nullptr),
      mWindow(nullptr),
      mController(nullptr),
      mHUD(nullptr),
      mUpgradeHUD(nullptr),
      mTutorialHUD(nullptr),
      mTicksCount(0),
      mIsRunning(true),
      mIsFullscreen(false),
      mIsPaused(false),
      mIsGameOver(false),
      mIsGameWon(false),
      mIsDebugging(false),
      mIsGodMode(false),
      mCurrentScene(GameScene::MainMenu)
{
}

Game::~Game()
{
    LevelManager::Instance().Shutdown();
}

bool Game::Initialize()
{
    Random::Init();

    if (!SystemInitializer::InitializeSDL()) 
        return false;

    mWindow = SystemInitializer::CreateGameWindow(GameConstants::WINDOW_WIDTH, GameConstants::WINDOW_HEIGHT);
    if (!mWindow) 
        return false;

    mRenderer = new Renderer(mWindow);
    mRenderer->Initialize(GameConstants::WINDOW_WIDTH, GameConstants::WINDOW_HEIGHT);

    mController = SystemInitializer::FindGameController();

    SDL_ShowCursor(SDL_DISABLE);

    mAudio = new AudioSystem();
    mAudio->CacheAllSounds();

    // Initialize level manager
    LevelManager::Instance().Initialize(this);

    // Register skills
    SkillFactory::InitializeSkills();

    SetScene(GameScene::MainMenu);

    mTicksCount = SDL_GetTicks();

    return true;
}

void Game::RunLoop()
{
    while (mIsRunning)
    {
        float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
        if (deltaTime > 0.05f)
        {
            deltaTime = 0.05f;
        }

        mTicksCount = SDL_GetTicks();

        ProcessInput();
        UpdateGame(deltaTime);
        GenerateOutput();

        int sleepTime = (1000 / GameConstants::FPS) - (SDL_GetTicks() - mTicksCount);
        if (sleepTime > 0)
        {
            SDL_Delay(sleepTime);
        }
    }
}

void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            Quit();
            break;

        case SDL_CONTROLLERDEVICEADDED:
            if (!mController)
            {
                mController = SDL_GameControllerOpen(event.cdevice.which);
                if (mController)
                    SDL_Log("Game controller added: %s", SDL_GameControllerName(mController));
            }
            break;

        case SDL_CONTROLLERDEVICEREMOVED:
            if (mController && SDL_GameControllerGetJoystick(mController) == SDL_JoystickFromInstanceID(event.cdevice.which))
            {
                SDL_Log("Game controller removed");
                SDL_GameControllerClose(mController);
                mController = nullptr;
            }
            break;

        case SDL_KEYDOWN:
        case SDL_MOUSEBUTTONDOWN:
            // UI input
            if (!mUIStack.empty())
            {
                mUIStack.back()->HandleKeyPress(event.key.keysym.sym);
            }

            // Fullscreen toggle
            if (event.key.keysym.sym == SDLK_F11 && event.key.repeat == 0)
            {
                mIsFullscreen = !mIsFullscreen;
                if (mIsFullscreen)
                {
                    SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
                    int w, h;
                    SDL_GetWindowSize(mWindow, &w, &h);
                    mRenderer->UpdateViewport(w, h);
                }
                else
                {
                    SDL_SetWindowFullscreen(mWindow, 0);
                    mRenderer->UpdateViewport(GameConstants::WINDOW_WIDTH, GameConstants::WINDOW_HEIGHT);
                }
            }

            // Debug toggle
            if (event.key.keysym.sym == SDLK_F1 && event.key.repeat == 0)
                mIsDebugging = !mIsDebugging;

            // Tutorial HUD toggle
            if (event.key.keysym.sym == SDLK_h && event.key.repeat == 0)
                if (mTutorialHUD)
                    mTutorialHUD->ToggleControlVisibility();

            // Pause toggle
            if (event.key.keysym.sym == SDLK_ESCAPE && event.key.repeat == 0)
            {
                if (mCurrentScene > GameScene::MainMenu && LevelManager::Instance().GetPlayer() && LevelManager::Instance().GetPlayer()->GetUpgradePoints() == 0)
                {
                    if (mIsPaused)
                        ResumeGame();
                    else
                        PauseGame();
                }
            }

            // Pass to actors
            for (auto actor : LevelManager::Instance().GetActors())
                actor->OnHandleEvent(event);
            break;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(nullptr);
    LevelManager::Instance().ProcessInput(state);
}

void Game::UpdateGame(float deltaTime)
{
    // End condition check
    if (mIsGameOver || mIsGameWon)
    {
        PauseGame();

        bool screenExists = false;
        for (auto screen : mUIStack)
        {
            if (dynamic_cast<GameOver*>(screen) != nullptr || dynamic_cast<WinScreen*>(screen) != nullptr)
            {
                screenExists = true;
                break;
            }
        }

        if (!screenExists)
        {
            if (mIsGameOver)
                new GameOver(this, "../Assets/Fonts/Pixellari.ttf");
            else
                new WinScreen(this, "../Assets/Fonts/Pixellari.ttf");
        }
    }

    // Update level (actors, camera, portal, transitions)
    LevelManager::Instance().Update(deltaTime);

    // Update audio
    if (mAudio)
        mAudio->Update(deltaTime);

    // Update UI
    for (auto ui : mUIStack)
    {
        if (ui->GetState() == UIScreen::UIState::Active)
        {
            ui->Update(deltaTime);
        }
    }

    // Remove closed UI screens
    auto iter = mUIStack.begin();
    while (iter != mUIStack.end())
    {
        if ((*iter)->GetState() == UIScreen::UIState::Closing)
        {
            delete *iter;
            iter = mUIStack.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void Game::GenerateOutput()
{
    mRenderer->Clear();

    // Get background texture
    std::string backgroundPath;
    switch (mCurrentScene)
    {
    case GameScene::MainMenu:
        backgroundPath = "../Assets/HUD/Background/MainMenuBackground.png";
        break;
    case GameScene::Lobby:
        backgroundPath = "../Assets/Levels/Lobby/LobbyBackground.png";
        break;
    case GameScene::Level1:
        backgroundPath = "../Assets/Levels/Level1/Level1Background.png";
        break;
    case GameScene::Level1_Boss:
        backgroundPath = "../Assets/Levels/Level1_Boss/Level1_Boss_Background.png";
        break;
    case GameScene::Level2:
        backgroundPath = "../Assets/Levels/Level2/Level2Background.png";
        break;
    case GameScene::Level2_Boss:
        backgroundPath = "../Assets/Levels/Level2_Boss/Level2_Boss_Background.png";
        break;
    case GameScene::Level3:
        backgroundPath = "../Assets/Levels/Level3/Level3Background.png";
        break;
    case GameScene::Level3_Boss:
        backgroundPath = "../Assets/Levels/Level3_Boss/Level3_Boss_Background.png";
        break;
    default:
        backgroundPath = "../Assets/Levels/Lobby/LobbyBackground.png";
        break;
    }

    Texture* backgroundTexture = mRenderer->GetTexture(backgroundPath);
    if (backgroundTexture)
    {
        // Main menu
        if (mCurrentScene == GameScene::MainMenu)
        {
            Vector2 position(GameConstants::WINDOW_WIDTH / 2.0f, GameConstants::WINDOW_HEIGHT / 2.0f);
            Vector2 size(static_cast<float>(backgroundTexture->GetWidth()), static_cast<float>(backgroundTexture->GetHeight()));
            mRenderer->DrawTexture(position, size, 0.0f, Vector3(1.0f, 1.0f, 1.0f), backgroundTexture, Vector4::UnitRect, Vector2::Zero);
        }
        else
        {
            // Scaled background for levels
            float levelPixelWidth = static_cast<float>(LevelManager::Instance().GetLevelWidth()) * static_cast<float>(GameConstants::TILE_SIZE);
            float levelPixelHeight = static_cast<float>(LevelManager::Instance().GetLevelHeight()) * static_cast<float>(GameConstants::TILE_SIZE);

            float texW = static_cast<float>(backgroundTexture->GetWidth());
            float texH = static_cast<float>(backgroundTexture->GetHeight());

            float scale = std::max(levelPixelWidth / texW, levelPixelHeight / texH);
            float backgroundWidth = texW * scale;
            float backgroundHeight = texH * scale;

            Vector2 position(levelPixelWidth / 2.0f, levelPixelHeight / 2.0f);
            Vector2 size(backgroundWidth, backgroundHeight);

            mRenderer->DrawTexture(position, size, 0.0f, Vector3(1.0f, 1.0f, 1.0f), backgroundTexture, Vector4::UnitRect, LevelManager::Instance().GetCameraPos());
        }
    }

    // Draw actors
    for (auto drawable : LevelManager::Instance().GetDrawables())
    {
        drawable->Draw(mRenderer);

        if (mIsDebugging)
        {
            auto actor = drawable->GetOwner();
            auto boss = dynamic_cast<Boss*>(actor);
            if (boss)
            {
                boss->OnDebugDraw(mRenderer);
            }

            for (auto comp : actor->GetComponents())
            {
                comp->DebugDraw(mRenderer);
            }
        }

        for (auto comp : drawable->GetOwner()->GetComponents())
            comp->ComponentDraw(mRenderer);
    }

    // Draw UI
    mRenderer->DrawAllUI();

    mRenderer->Present();
}

void Game::SetScene(GameScene nextScene)
{
    UnloadScene();
    mCurrentScene = nextScene;

    switch (nextScene)
    {
    case GameScene::MainMenu:
        mBackgroundMusic = mAudio->PlaySound("m01_main_menu.mp3", true, 0.3f);
        new MainMenu(this, "../Assets/Fonts/Pixellari.ttf");
        break;

    case GameScene::Lobby:
        mBackgroundMusic = mAudio->PlaySound("m04_tutorial.mp3", true, 0.5f);
        
        if (!mTutorialHUD)
            mTutorialHUD = new TutorialHUD(this, "../Assets/Fonts/Pixellari.ttf");

        mHUD = new HUD(this, "../Assets/Fonts/Pixellari.ttf");
        mUpgradeHUD = new UpgradeHUD(this, "../Assets/Fonts/Pixellari.ttf");

        if (mTutorialHUD)
            mTutorialHUD->ShowControls();

        LevelManager::Instance().LoadAndBuildLevel(nextScene);
        break;

    case GameScene::Level1:
        mBackgroundMusic = mAudio->PlaySound("m05_level1.mp3", true, 0.5f);
        if (mTutorialHUD)
            mTutorialHUD->HideControls();
        LevelManager::Instance().LoadAndBuildLevel(nextScene);
        break;

    case GameScene::Level1_Boss:
        mBackgroundMusic = mAudio->PlaySound("m08_boss1_grass.mp3", true, 0.3f);
        if (mTutorialHUD)
            mTutorialHUD->HideControls();
        LevelManager::Instance().LoadAndBuildLevel(nextScene);
        break;

    case GameScene::Level2:
        mBackgroundMusic = mAudio->PlaySound("m06_level2.mp3", true, 0.6f);
        if (mTutorialHUD)
            mTutorialHUD->HideControls();
        LevelManager::Instance().LoadAndBuildLevel(nextScene);
        break;

    case GameScene::Level2_Boss:
        mBackgroundMusic = mAudio->PlaySound("m09_boss2_bricks.mp3", true);
        if (mTutorialHUD)
            mTutorialHUD->HideControls();
        LevelManager::Instance().LoadAndBuildLevel(nextScene);
        break;

    case GameScene::Level3:
        mBackgroundMusic = mAudio->PlaySound("m07_level3.mp3", true);
        if (mTutorialHUD)
            mTutorialHUD->HideControls();
        LevelManager::Instance().LoadAndBuildLevel(nextScene);
        break;

    case GameScene::Level3_Boss:
        mBackgroundMusic = mAudio->PlaySound("m10_boss3_stone.mp3", true);
        if (mTutorialHUD)
            mTutorialHUD->HideControls();
        LevelManager::Instance().LoadAndBuildLevel(nextScene);
        break;
    }
}

void Game::UnloadScene()
{
    if (mBackgroundMusic.IsValid())
    {
        mAudio->StopSound(mBackgroundMusic);
        mBackgroundMusic.Reset();
    }

    LevelManager::Instance().UnloadLevel();

    // Delete UI screens (except persistent HUDs)
    for (auto ui : mUIStack)
    {
        if (ui == mHUD || ui == mTutorialHUD || ui == mUpgradeHUD)
            continue;
        delete ui;
    }
    mUIStack.clear();
    
    if (mTutorialHUD)
        mUIStack.push_back(mTutorialHUD);
    if (mHUD)
        mUIStack.push_back(mHUD);
    if (mUpgradeHUD)
        mUIStack.push_back(mUpgradeHUD);
}

void Game::PauseGame()
{
    mIsPaused = true;
    for (auto actor : LevelManager::Instance().GetActors())
        actor->SetState(ActorState::Paused);
}

void Game::ResumeGame()
{
    mIsPaused = false;
    for (auto actor : LevelManager::Instance().GetActors())
        actor->SetState(ActorState::Active);
}

void Game::ResetGame()
{
    mAudio->StopAllSounds();

    auto iter = mUIStack.begin();
    while (iter != mUIStack.end())
    {
        if ((*iter)->GetState() == UIScreen::UIState::Closing)
        {
            delete *iter;
            iter = mUIStack.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    SetGameOver(false);
    SetGameWon(false);
    SetScene(GameScene::Level1);

    LevelManager::Instance().GetPlayer()->SetHP(LevelManager::Instance().GetPlayer()->GetMaxHP());

    ResumeGame();
}

GroundType Game::GetGroundType() const
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

Vector2 Game::GetMouseWorldPosition()
{
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    float scaleX = static_cast<float>(GameConstants::WINDOW_WIDTH) / static_cast<float>(mRenderer->GetScreenWidth());
    float scaleY = static_cast<float>(GameConstants::WINDOW_HEIGHT) / static_cast<float>(mRenderer->GetScreenHeight());
    mouseX = static_cast<int>(mouseX * scaleX);
    mouseY = static_cast<int>(mouseY * scaleY);

    Vector2 worldPos;
    worldPos.x = static_cast<float>(mouseX) + LevelManager::Instance().GetCameraPos().x;
    worldPos.y = static_cast<float>(mouseY) + LevelManager::Instance().GetCameraPos().y;

    return worldPos;
}

Vector2 Game::GetMouseAbsolutePosition()
{
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    float scaleX = static_cast<float>(GameConstants::WINDOW_WIDTH) / static_cast<float>(mRenderer->GetScreenWidth());
    float scaleY = static_cast<float>(GameConstants::WINDOW_HEIGHT) / static_cast<float>(mRenderer->GetScreenHeight());
    
    return Vector2(static_cast<float>(mouseX * scaleX), static_cast<float>(mouseY * scaleY));
}

void Game::Shutdown()
{
    LevelManager::Instance().Shutdown();

    for (auto ui : mUIStack)
    {
        delete ui;
    }
    mUIStack.clear();

    mRenderer->Shutdown();
    delete mRenderer;
    mRenderer = nullptr;

    delete mAudio;
    mAudio = nullptr;

    if (mController)
    {
        SDL_GameControllerClose(mController);
        mController = nullptr;
    }

    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}