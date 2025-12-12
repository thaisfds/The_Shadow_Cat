#pragma once

class Game;

class GameRenderer
{
public:
    // Singleton access
    static GameRenderer& Instance();

    // Delete copy constructor and assignment operator
    GameRenderer(const GameRenderer&) = delete;
    GameRenderer& operator=(const GameRenderer&) = delete;

    // Initialize with game reference
    void Initialize(Game* game);
    void Shutdown();

    // Main rendering
    void Render();

private:
    // Private constructor for singleton
    GameRenderer();
    ~GameRenderer();

    // Rendering helpers
    void DrawBackground();
    void DrawActors();

    // Game reference
    Game* mGame;
};
