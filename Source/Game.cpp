#include "Game.h"
#include "LevelManager.h"
#include "InputHandler.h"
#include "SceneManager.h"
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
      mIsGodMode(false)
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

    // Initialize input handler
    InputHandler::Instance().Initialize(this);

    SDL_ShowCursor(SDL_DISABLE);

    mAudio = new AudioSystem();
    mAudio->CacheAllSounds();

    // Initialize managers
    LevelManager::Instance().Initialize(this);
    SceneManager::Instance().Initialize(this);

    // Register skills
    SkillFactory::InitializeSkills();

    SceneManager::Instance().SetScene(GameScene::MainMenu);

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

        InputHandler::Instance().ProcessInput();
        UpdateGame(deltaTime);
        GenerateOutput();

        int sleepTime = (1000 / GameConstants::FPS) - (SDL_GetTicks() - mTicksCount);
        if (sleepTime > 0)
        {
            SDL_Delay(sleepTime);
        }
    }
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
    std::string backgroundPath = SceneManager::Instance().GetBackgroundPath();
    Texture* backgroundTexture = mRenderer->GetTexture(backgroundPath);
    if (backgroundTexture)
    {
        // Main menu
        if (SceneManager::Instance().GetCurrentScene() == GameScene::MainMenu)
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
    SceneManager::Instance().SetScene(GameScene::Level1);

    LevelManager::Instance().GetPlayer()->SetHP(LevelManager::Instance().GetPlayer()->GetMaxHP());

    ResumeGame();
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
    SceneManager::Instance().Shutdown();
    InputHandler::Instance().Shutdown();
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

    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}