#pragma once
#include <SDL.h>
#include <vector>

#include "AudioSystem.h"
#include "Math.h"

enum class GameScene
{
    MainMenu,
    Lobby,
    Level1,
    Level1_Boss,
    Level2,
    Level2_Boss,
    Level3,
    Level3_Boss
};

enum class GroundType
{
    Grass,
    Brick,
    Stone
};

class Game
{
public:
    Game();
    ~Game();

    bool Initialize();
    void RunLoop();
    void Shutdown();
    void Quit() { mIsRunning = false; }

    // Scene Handling
    void SetScene(GameScene scene);
    GroundType GetGroundType() const;
    GameScene GetCurrentScene() const { return mCurrentScene; }

    // UI Management
    void PushUI(class UIScreen* screen) { mUIStack.emplace_back(screen); }
    const std::vector<class UIScreen*>& GetUIStack() { return mUIStack; }

    // Game State
    void PauseGame();
    void ResumeGame();
    void ResetGame();
    bool IsPaused() const { return mIsPaused; }
    void SetGameOver(bool isOver) { mIsGameOver = isOver; }
    void SetGameWon(bool isWon) { mIsGameWon = isWon; }

    // System Access
    class Renderer* GetRenderer() { return mRenderer; }
    class AudioSystem* GetAudio() { return mAudio; }
    SDL_Window* GetWindow() { return mWindow; }
    
    // UI Access
    class HUD* GetHUD() { return mHUD; }
    class TutorialHUD* GetTutorialHUD() { return mTutorialHUD; }

    // Debug
    bool IsDebugging() const { return mIsDebugging; }
    void SetDebugging(bool debug) { mIsDebugging = debug; }

    // Fullscreen
    bool IsFullscreen() const { return mIsFullscreen; }
    void SetFullscreen(bool fullscreen) { mIsFullscreen = fullscreen; }

    // Mouse position (used by UI)
    Vector2 GetMouseWorldPosition();
    Vector2 GetMouseAbsolutePosition();

private:
    void ProcessInput();
    void UpdateGame(float deltaTime);
    void GenerateOutput();
    void UnloadScene();

    // Systems
    class Renderer* mRenderer;
    class AudioSystem* mAudio;

    // SDL
    SDL_Window* mWindow;

    // UI
    std::vector<class UIScreen*> mUIStack;
    class HUD* mHUD;
    class UpgradeHUD* mUpgradeHUD;
    class TutorialHUD* mTutorialHUD;

    // Audio
    SoundHandle mBackgroundMusic;

    // Game state
    Uint32 mTicksCount;
    bool mIsRunning;
    bool mIsFullscreen;
    bool mIsPaused;
    bool mIsGameOver;
    bool mIsGameWon;
    bool mIsDebugging;
    bool mIsGodMode;
    GameScene mCurrentScene;
};