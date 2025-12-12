#pragma once
#include <SDL.h>

class Game;

class InputHandler
{
public:
    // Singleton access
    static InputHandler& Instance();

    // Delete copy constructor and assignment operator
    InputHandler(const InputHandler&) = delete;
    InputHandler& operator=(const InputHandler&) = delete;

    // Initialize with game reference
    void Initialize(Game* game);
    void Shutdown();

    // Main input processing
    void ProcessInput();

    // Controller management
    void HandleControllerAdded(int deviceIndex);
    void HandleControllerRemoved(SDL_JoystickID instanceID);
    SDL_GameController* GetController() { return mController; }

private:
    // Private constructor for singleton
    InputHandler();
    ~InputHandler();

    // Event handlers
    void HandleKeyPress(SDL_Keycode key, Uint8 repeat);
    void HandleMouseButton();

    // Game reference
    Game* mGame;

    // Controller
    SDL_GameController* mController;
};
