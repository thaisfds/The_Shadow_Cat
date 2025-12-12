#include "SystemInitializer.h"
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "GameConstants.h"

bool SystemInitializer::InitializeSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	// Init SDL Image
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		SDL_Log("Unable to initialize SDL_image: %s", IMG_GetError());
		return false;
	}

	// Initialize SDL_ttf
	if (TTF_Init() != 0)
	{
		SDL_Log("Failed to initialize SDL_ttf");
		return false;
	}

	// Initialize SDL_mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
	{
		SDL_Log("Failed to initialize SDL_mixer");
		return false;
	}

	return true;
}

SDL_Window* SystemInitializer::CreateGameWindow(int width, int height)
{
	SDL_Window* window = SDL_CreateWindow("The Shadow Cat", 
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
		width, height, SDL_WINDOW_OPENGL);
	
	if (!window)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return nullptr;
	}

	return window;
}

SDL_GameController* SystemInitializer::FindGameController()
{
	for (int i = 0; i < SDL_NumJoysticks(); ++i)
	{
		if (SDL_IsGameController(i))
		{
			SDL_GameController* controller = SDL_GameControllerOpen(i);
			if (controller)
			{
				SDL_Log("Found game controller: %s", SDL_GameControllerName(controller));
				return controller;
			}
			else
			{
				SDL_Log("Could not open game controller %i: %s", i, SDL_GetError());
			}
		}
	}
	return nullptr;
}