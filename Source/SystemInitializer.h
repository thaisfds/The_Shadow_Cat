#pragma once

#include <SDL.h>

class Renderer;

class SystemInitializer
{
public:
	static bool InitializeSDL();
	static SDL_Window* CreateGameWindow(int width, int height);
	static SDL_GameController* FindGameController();
	static void Shutdown(SDL_Window* window, SDL_GameController* controller);
};