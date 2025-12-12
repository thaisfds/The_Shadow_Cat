#pragma once
#include "AudioSystem.h"
#include <string>

class Game;

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

class SceneManager
{
public:
    // Singleton access
    static SceneManager& Instance();

    // Delete copy constructor and assignment operator
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

    // Initialize with game reference
    void Initialize(Game* game);
    void Shutdown();

    // Scene management
    void SetScene(GameScene scene);
    void UnloadScene();
    
    // Scene info
    GameScene GetCurrentScene() const { return mCurrentScene; }
    GroundType GetGroundType() const;
    std::string GetBackgroundPath() const;

private:
    // Private constructor for singleton
    SceneManager();
    ~SceneManager();

    // Game reference
    Game* mGame;

    // Current scene state
    GameScene mCurrentScene;
    SoundHandle mBackgroundMusic;
};
