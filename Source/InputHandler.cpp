#include "InputHandler.h"
#include "Game.h"
#include "LevelManager.h"
#include "GameConstants.h"
#include <SDL.h>
#include "UI/Screens/UIScreen.h"
#include "Renderer/Renderer.h"
#include "UI/Screens/TutorialHUD.h"
#include "Actors/Characters/ShadowCat.h"

// Singleton instance
InputHandler& InputHandler::Instance()
{
    static InputHandler instance;
    return instance;
}

InputHandler::InputHandler()
    : mGame(nullptr),
      mController(nullptr)
{
}

InputHandler::~InputHandler()
{
    Shutdown();
}

void InputHandler::Initialize(Game* game)
{
    mGame = game;

    // Check for existing game controllers
    for (int i = 0; i < SDL_NumJoysticks(); ++i)
    {
        if (SDL_IsGameController(i))
        {
            HandleControllerAdded(i);
            if (mController)
                break; // Only open one controller
        }
    }
}

void InputHandler::Shutdown()
{
    if (mController)
    {
        SDL_GameControllerClose(mController);
        mController = nullptr;
    }
    mGame = nullptr;
}

void InputHandler::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            mGame->Quit();
            break;

        case SDL_CONTROLLERDEVICEADDED:
            if (!mController)
            {
                HandleControllerAdded(event.cdevice.which);
            }
            break;

        case SDL_CONTROLLERDEVICEREMOVED:
            HandleControllerRemoved(event.cdevice.which);
            break;

        case SDL_KEYDOWN:
        case SDL_MOUSEBUTTONDOWN:
            // Handle UI and system keys first
            if (event.type == SDL_KEYDOWN)
            {
                HandleKeyPress(event.key.keysym.sym, event.key.repeat);
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                HandleMouseButton();
            }

            // Pass the original event to all actors (for skills, etc.)
            for (auto actor : LevelManager::Instance().GetActors())
            {
                actor->OnHandleEvent(event);
            }
            break;
        }
    }

    // Process continuous keyboard input for actors
    const Uint8* state = SDL_GetKeyboardState(nullptr);
    LevelManager::Instance().ProcessInput(state);
}

void InputHandler::HandleControllerAdded(int deviceIndex)
{
    mController = SDL_GameControllerOpen(deviceIndex);
    if (mController)
    {
        SDL_Log("Game controller added: %s", SDL_GameControllerName(mController));
    }
    else
    {
        SDL_Log("Could not open game controller %i: %s", deviceIndex, SDL_GetError());
    }
}

void InputHandler::HandleControllerRemoved(SDL_JoystickID instanceID)
{
    if (mController && SDL_GameControllerGetJoystick(mController) == SDL_JoystickFromInstanceID(instanceID))
    {
        SDL_Log("Game controller removed");
        SDL_GameControllerClose(mController);
        mController = nullptr;
    }
}

void InputHandler::HandleKeyPress(SDL_Keycode key, Uint8 repeat)
{
    // UI input
    if (!mGame->GetUIStack().empty())
    {
        mGame->GetUIStack().back()->HandleKeyPress(key);
    }

    // Fullscreen toggle (F11) - only on initial press
    if (key == SDLK_F11 && repeat == 0)
    {
        bool isFullscreen = mGame->IsFullscreen();
        mGame->SetFullscreen(!isFullscreen);

        if (!isFullscreen)
        {
            SDL_SetWindowFullscreen(mGame->GetWindow(), SDL_WINDOW_FULLSCREEN_DESKTOP);
            int w, h;
            SDL_GetWindowSize(mGame->GetWindow(), &w, &h);
            mGame->GetRenderer()->UpdateViewport(w, h);
        }
        else
        {
            SDL_SetWindowFullscreen(mGame->GetWindow(), 0);
            mGame->GetRenderer()->UpdateViewport(GameConstants::WINDOW_WIDTH, GameConstants::WINDOW_HEIGHT);
        }
    }

    // Debug toggle (F1) - only on initial press
    if (key == SDLK_F1 && repeat == 0)
    {
        mGame->SetDebugging(!mGame->IsDebugging());
    }

    // Tutorial HUD toggle (H) - only on initial press
    if (key == SDLK_h && repeat == 0)
    {
        auto tutorialHUD = mGame->GetTutorialHUD();
        if (tutorialHUD)
            tutorialHUD->ToggleControlVisibility();
    }

    // Pause toggle (ESC) - only on initial press
    if (key == SDLK_ESCAPE && repeat == 0)
    {
        if (mGame->GetCurrentScene() > GameScene::MainMenu && 
            LevelManager::Instance().GetPlayer() && 
            LevelManager::Instance().GetPlayer()->GetUpgradePoints() == 0)
        {
            if (mGame->IsPaused())
                mGame->ResumeGame();
            else
                mGame->PauseGame();
        }
    }
}

void InputHandler::HandleMouseButton()
{
    // UI input
    if (!mGame->GetUIStack().empty())
    {
        mGame->GetUIStack().back()->HandleKeyPress(SDLK_UNKNOWN); // Mouse clicks
    }
}