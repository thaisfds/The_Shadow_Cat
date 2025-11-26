#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>
#include "Renderer/Renderer.h"
#include "AudioSystem.h"

enum class GameScene
{
    MainMenu,
    Lobby,
    Level1,
    GameOver
};

class Game
{
public:
    Game();

    bool Initialize();
    void RunLoop();
    void Shutdown();
    void Quit() { mIsRunning = false; }

    // Actor functions
    void InitializeActors();
    void UpdateActors(float deltaTime);
    void AddActor(class Actor *actor);
    void RemoveActor(class Actor *actor);

    // UI functions
    void PushUI(class UIScreen* screen) { mUIStack.emplace_back(screen); }
    const std::vector<class UIScreen*>& GetUIStack() { return mUIStack; }

    // Audio
    AudioSystem* GetAudio() { return mAudio; }

    // Scene Handling
    void SetScene(GameScene scene);
    void UnloadScene();

    // Renderer
    class Renderer *GetRenderer() { return mRenderer; }

    // Draw functions
    void AddDrawable(class DrawComponent *drawable);
    void RemoveDrawable(class DrawComponent *drawable);
    std::vector<class DrawComponent *> &GetDrawables() { return mDrawables; }

    // Collider functions
    void AddCollider(class AABBColliderComponent *collider);
    void RemoveCollider(class AABBColliderComponent *collider);
    std::vector<class AABBColliderComponent *> &GetColliders() { return mColliders; }

    // Camera functions
    Vector2 &GetCameraPos() { return mCameraPos; };
    void SetCameraPos(const Vector2 &position) { mCameraPos = position; };

    // Game specific
    const class ShadowCat *GetPlayer() { return mShadowCat; }
    class HUD* GetHUD() { return mHUD; }

    SDL_GameController *mController;

private:
    void ProcessInput();
    void UpdateGame(float deltaTime);
    void UpdateCamera();
    void GenerateOutput();

    // Level loading
    int **LoadLevel(const std::string &fileName, int width, int height);
    void BuildLevel(int **levelData, int width, int height);

    // All the actors in the game
    std::vector<class Actor *> mActors;
    std::vector<class Actor *> mPendingActors;

    // Camera
    Vector2 mCameraPos;

    // All the draw components
    std::vector<class DrawComponent *> mDrawables;

    // All the collision components
    std::vector<class AABBColliderComponent *> mColliders;

    // All UI screens in the game
    std::vector<class UIScreen*> mUIStack;

    // SDL stuff
    SDL_Window *mWindow;
    class Renderer *mRenderer;

    // Audio system
    AudioSystem* mAudio;

    // Track elapsed time since game start
    Uint32 mTicksCount;

    // Track if we're updating actors right now
    bool mIsRunning;
    bool mIsDebugging;
    bool mUpdatingActors;
    bool mIsFullscreen;
    GameScene mCurrentScene;

    // Game-specific
    class ShadowCat *mShadowCat;
    class HUD *mHUD;
    int **mLevelData;
};
