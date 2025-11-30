#include <algorithm>
#include <vector>
#include <map>
#include <fstream>
#include "Actors/Characters/Dummy.h"
#include "CSV.h"
#include "Game.h"
#include "GameConstants.h"
#include "Components/Drawing/DrawComponent.h"
#include "Components/Physics/RigidBodyComponent.h"
#include "Random.h"
#include "UI/Screens/HUD.h"
#include "UI/Screens/MainMenu.h"
#include "Actors/Actor.h"
#include "Actors/Block.h"
#include "Actors/Spawner.h"
#include "Actors/Characters/ShadowCat.h"
#include "Components/AnimatedParticleSystemComponent.h"
#include "Actors/Characters/BasicEnemy.h"

Game::Game()
	: mWindow(nullptr),
	mRenderer(nullptr),
	mTicksCount(0),
	mIsRunning(true),
	mIsDebugging(true),
	mUpdatingActors(false),
	mCameraPos(Vector2::Zero),
	mLevelData(nullptr),
	mAudio(nullptr),
    mHUD(nullptr),
	mShadowCat(nullptr),
	mController(nullptr),
	mLevelWidth(0),
	mLevelHeight(0)
{
}

bool Game::Initialize()
{
	Random::Init();

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

	mWindow = SDL_CreateWindow("The Shadow Cat", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, GameConstants::WINDOW_WIDTH, GameConstants::WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	mRenderer = new Renderer(mWindow);
	mRenderer->Initialize(GameConstants::WINDOW_WIDTH, GameConstants::WINDOW_HEIGHT);

	for (int i = 0; i < SDL_NumJoysticks(); ++i)
	{
		if (SDL_IsGameController(i))
		{
			mController = SDL_GameControllerOpen(i);
			if (mController)
			{
				SDL_Log("Found game controller: %s", SDL_GameControllerName(mController));
				break;
			}
			else
			{
				SDL_Log("Could not open game controller %i: %s", i, SDL_GetError());
			}
		}
	}

	// Init audio system
	mAudio = new AudioSystem();
    mAudio->CacheAllSounds();

	// First scene
		SetScene(GameScene::Level1);

	mTicksCount = SDL_GetTicks();

	return true;
}

void Game::UnloadScene()
{
    // Use state so we can call this from within an actor update
    for(auto *actor : mActors) {
        actor->SetState(ActorState::Destroy);
    }

    // Delete UI screens
    for (auto ui : mUIStack) {
        delete ui;
    }
    mUIStack.clear();

    // Reset states
    mHUD = nullptr;
	mShadowCat = nullptr;
}

void Game::SetScene(GameScene nextScene)
{
    UnloadScene();

    switch (nextScene) {
        case GameScene::MainMenu:
            mCurrentScene = GameScene::MainMenu;
			
			// Main menu back music
			// mAudio->PlaySound("Music.ogg", true);

			// Still debugging this
			new MainMenu(this, "../Assets/Fonts/Pixellari.ttf");
            break;

        case GameScene::Lobby:
            mCurrentScene = GameScene::Lobby;
            InitializeActors();
            break;

        case GameScene::Level1:
            mCurrentScene = GameScene::Level1;
            InitializeActors();
            break;
    }
}

void Game::InitializeActors()
{
	// Initialize debug actor
	mDebugActor = new DebugActor(this);

	mAttackTrailActor = new Actor(this);
    new AnimatedParticleSystemComponent(mAttackTrailActor, "AttackTrailAnim", false);

	std::string levelPath;
	
	// Choose level based on current scene
	if (mCurrentScene == GameScene::Lobby) {
		levelPath = "../Assets/Levels/Lobby/Lobby.csv";
	} else if (mCurrentScene == GameScene::Level1) {
		levelPath = "../Assets/Levels/Level1/Level1.csv";
	} else {
		levelPath = "../Assets/Levels/Lobby/Lobby.csv";
	}

	mLevelData = LoadLevel(levelPath, mLevelWidth, mLevelHeight);

	if (mLevelData)
	{
		BuildLevel(mLevelData, mLevelWidth, mLevelHeight);
	}
}

int **Game::LoadLevel(const std::string &fileName, int &outWidth, int &outHeight)
{
	std::ifstream levelFile(fileName);
	if (!levelFile.is_open())
	{
		SDL_Log("Failed to open level file: %s", fileName.c_str());
		return nullptr;
	}

	// First pass: read all data and determine dimensions
	std::vector<std::vector<int>> tempData;
	std::string line;
	int maxWidth = 0;

	while (std::getline(levelFile, line))
	{
		std::vector<int> row = CSVHelper::Split(line);
		if (row.size() > maxWidth)
		{
			maxWidth = row.size();
		}
		tempData.push_back(row);
	}

	outHeight = tempData.size();
	outWidth = maxWidth;

	if (outHeight == 0 || outWidth == 0)
	{
		SDL_Log("Invalid level dimensions: %d x %d", outWidth, outHeight);
		return nullptr;
	}

	// Allocate level data array
	int **levelData = new int *[outHeight];
	for (int i = 0; i < outHeight; ++i)
	{
		levelData[i] = new int[outWidth];
	}

	// Fill level data from temp data
	for (int i = 0; i < outHeight; ++i)
	{
		for (int j = 0; j < outWidth; ++j)
		{
			if (j < tempData[i].size())
			{
				levelData[i][j] = tempData[i][j];
			}
			else
			{
				levelData[i][j] = 0;
			}
		}
	}

	if (mIsDebugging)
	{
		SDL_Log("--- Level CSV Content (%d x %d) ---", outWidth, outHeight);
		for (int i = 0; i < outHeight; ++i)
		{
			std::string rowStr = "";
			for (int j = 0; j < outWidth; ++j)
			{
				rowStr += std::to_string(levelData[i][j]) + " ";
			}
			SDL_Log("%s", rowStr.c_str());
		}
		SDL_Log("-----------------------------");
	}

	return levelData;
}

void Game::BuildLevel(int **levelData, int width, int height)
{
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			int tileID = levelData[i][j];

			float x = (j * GameConstants::TILE_SIZE) + (GameConstants::TILE_SIZE / 2.0f);
			float y = (i * GameConstants::TILE_SIZE) + (GameConstants::TILE_SIZE / 2.0f);

			Vector2 position(x, y);

			// Player spawn
			if (tileID == 0)
			{
				mShadowCat = new ShadowCat(this);
				mShadowCat->SetPosition(position);
			}
			// Blocks
			else if (tileID >= 4 && tileID <= 10)
			{
				auto block = new Block(this, tileID);
				block->SetPosition(position);
			}
			// Dummy
			else if (tileID == 11)
			{
				auto dummy = new Dummy(this);
				dummy->SetPosition(position);
			}
			// BasicEnemy (WhiteCat)
			else if (tileID == 12)
			{
				auto enemy = new BasicEnemy(this, 0.0f, 100.0f);
				enemy->SetPosition(position);
			}
			// BasicEnemy with larger patrol (WhiteCat2)
			else if (tileID == 13)
			{
				auto enemy = new BasicEnemy(this, 0.0f, 400.0f);
				enemy->SetPosition(position);
			}
		}
	}
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		// Calculate delta time in seconds
		float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
		if (deltaTime > 0.05f)
		{
			deltaTime = 0.05f;
		}

		mTicksCount = SDL_GetTicks();

		ProcessInput();
		UpdateGame(deltaTime);
		GenerateOutput();

		// Sleep to maintain frame rate
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
			// Try to open the added controller if there isn't one already open
			if (!mController)
			{
				mController = SDL_GameControllerOpen(event.cdevice.which);
				if (mController)
				{
					SDL_Log("Game controller added: %s", SDL_GameControllerName(mController));
				}
				else
				{
					SDL_Log("Could not open new game controller: %s", SDL_GetError());
				}
			}
			break;
		case SDL_CONTROLLERDEVICEREMOVED:
			// Close the controller if it's the one that was removed
			if (mController && SDL_GameControllerGetJoystick(mController) == SDL_JoystickFromInstanceID(event.cdevice.which))
			{
				SDL_Log("Game controller removed");
				SDL_GameControllerClose(mController);
				mController = nullptr;
		}
			break;
		case SDL_KEYDOWN:
			// Handle key press for UI screens
			if (!mUIStack.empty()) {
				mUIStack.back()->HandleKeyPress(event.key.keysym.sym);
			}

			// Fullscreen toggle
			if (event.key.keysym.sym == SDLK_F11 && event.key.repeat == 0) {
				mIsFullscreen = !mIsFullscreen;
				if (mIsFullscreen)
				{
					SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);

					// Update viewport for fullscreen
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

			// Pass event to actors
			for (auto actor : mActors) actor->OnHandleEvent(event);
			break;
		}
	}

	const Uint8 *state = SDL_GetKeyboardState(nullptr);

	for (auto actor : mActors)
	{
		actor->ProcessInput(state);
	}
}

void Game::UpdateGame(float deltaTime)
{
	// Update all actors and pending actors
	UpdateActors(deltaTime);

	// Update camera position
	UpdateCamera();

	// Audio and UI
	if (mAudio)
        mAudio->Update(deltaTime);

    // Update UI screens
    for (auto ui : mUIStack) {
        if (ui->GetState() == UIScreen::UIState::Active) {
            ui->Update(deltaTime);
        }
    }

    // Delete any UI that are closed
    auto iter = mUIStack.begin();
    while (iter != mUIStack.end()) {
        if ((*iter)->GetState() == UIScreen::UIState::Closing) {
            delete *iter;
            iter = mUIStack.erase(iter);
        } else {
            ++iter;
        }
    }
}

void Game::UpdateActors(float deltaTime)
{
	mUpdatingActors = true;
	for (auto actor : mActors)
	{
		actor->Update(deltaTime);
	}
	mUpdatingActors = false;

	for (auto pending : mPendingActors)
	{
		mActors.emplace_back(pending);
	}
	mPendingActors.clear();

	std::vector<Actor *> deadActors;
	for (auto actor : mActors)
	{
		if (actor->GetState() == ActorState::Destroy)
		{
			deadActors.emplace_back(actor);
		}
	}

	for (auto actor : deadActors)
	{
		delete actor;
	}
}

void Game::UpdateCamera()
{
	if (mShadowCat)
	{
		float targetX = mShadowCat->GetPosition().x;
		float targetY = mShadowCat->GetPosition().y;

		// Center camera on ShadowCat by subtracting half of window dimensions
		mCameraPos.x = targetX - (GameConstants::WINDOW_WIDTH / 2.0f);
		mCameraPos.y = targetY - (GameConstants::WINDOW_HEIGHT / 2.0f);

		// Clamp camera to level boundaries
		float levelPixelWidth = static_cast<float>(mLevelWidth) * static_cast<float>(GameConstants::TILE_SIZE);
		float levelPixelHeight = static_cast<float>(mLevelHeight) * static_cast<float>(GameConstants::TILE_SIZE);

		// Prevent camera from showing area outside the level
		float maxCameraX = levelPixelWidth - GameConstants::WINDOW_WIDTH;
		float maxCameraY = levelPixelHeight - GameConstants::WINDOW_HEIGHT;

		// Clamp camera position
		if (mCameraPos.x < 0.0f)
			mCameraPos.x = 0.0f;
		if (mCameraPos.y < 0.0f)
			mCameraPos.y = 0.0f;
		if (mCameraPos.x > maxCameraX)
			mCameraPos.x = maxCameraX;
		if (mCameraPos.y > maxCameraY)
			mCameraPos.y = maxCameraY;
	}
}

void Game::AddActor(Actor *actor)
{
	if (mUpdatingActors)
	{
		mPendingActors.emplace_back(actor);
	}
	else
	{
		mActors.emplace_back(actor);
	}
}

void Game::RemoveActor(Actor *actor)
{
	auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
	if (iter != mPendingActors.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, mPendingActors.end() - 1);
		mPendingActors.pop_back();
	}

	iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, mActors.end() - 1);
		mActors.pop_back();
	}
}

void Game::AddDrawable(class DrawComponent *drawable)
{
	mDrawables.emplace_back(drawable);

	std::sort(mDrawables.begin(), mDrawables.end(), [](DrawComponent *a, DrawComponent *b)
			  { return a->GetDrawOrder() < b->GetDrawOrder(); });
}

void Game::RemoveDrawable(class DrawComponent *drawable)
{
	auto iter = std::find(mDrawables.begin(), mDrawables.end(), drawable);
	mDrawables.erase(iter);
}

void Game::AddCollider(class AABBColliderComponent *collider)
{
	mColliders.emplace_back(collider);
}

void Game::RemoveCollider(AABBColliderComponent *collider)
{
	auto iter = std::find(mColliders.begin(), mColliders.end(), collider);
	mColliders.erase(iter);
}

void Game::GenerateOutput()
{
	// Clear back buffer
	mRenderer->Clear();

	// Get background texture based on current scene
	std::string backgroundPath;
	if (mCurrentScene == GameScene::Lobby) {
		backgroundPath = "../Assets/Levels/Lobby/LobbyBackground.png";
	} else if (mCurrentScene == GameScene::Level1) {
		backgroundPath = "../Assets/Levels/Level1/Level1Background.png";
	} else {
		backgroundPath = "../Assets/Levels/Lobby/LobbyBackground.png";
	}

	Texture *backgroundTexture = mRenderer->GetTexture(backgroundPath);
	if (backgroundTexture)
	{
		float levelPixelWidth = static_cast<float>(mLevelWidth) * static_cast<float>(GameConstants::TILE_SIZE);
		float levelPixelHeight = static_cast<float>(mLevelHeight) * static_cast<float>(GameConstants::TILE_SIZE);

		float desiredWidth = levelPixelWidth;
		float desiredHeight = levelPixelHeight;

		float texW = static_cast<float>(backgroundTexture->GetWidth());
		float texH = static_cast<float>(backgroundTexture->GetHeight());

		float scale = 1.0f;
		if (texW > 0.0f && texH > 0.0f)
		{
			scale = std::max(desiredWidth / texW, desiredHeight / texH);
		}

		float backgroundWidth = texW * scale;
		float backgroundHeight = texH * scale;

		Vector2 position(levelPixelWidth / 2.0f, levelPixelHeight / 2.0f);
		Vector2 size(backgroundWidth, backgroundHeight);

		mRenderer->DrawTexture(position, size, 0.0f, Vector3(1.0f, 1.0f, 1.0f),
							   backgroundTexture, Vector4::UnitRect, mCameraPos);
	}

	for (auto drawable : mDrawables)
	{
		drawable->Draw(mRenderer);

		if (mIsDebugging)
		{
			// Call debug draw for actor
			auto actor = drawable->GetOwner();
			
			// Check if actor is a BasicEnemy and call its debug draw
			auto basicEnemy = dynamic_cast<BasicEnemy*>(actor);
			if (basicEnemy)
			{
				basicEnemy->OnDebugDraw(mRenderer);
			}
			
			// Call debug draw for actor components
			for (auto comp : actor->GetComponents())
			{
				comp->DebugDraw(mRenderer);
			}
		}
	}

	// Draw UI (TODO: unify in a single draw function and remove mDrawables, add to renderer)
	mRenderer->DrawAllUI();

	// Swap front buffer and back buffer
	mRenderer->Present();
}

void Game::Shutdown()
{
	while (!mActors.empty())
	{
		delete mActors.back();
	}

	if (mController)
	{
		SDL_GameControllerClose(mController);
		mController = nullptr;
	}

	// Delete level data
	if (mLevelData)
	{
		for (int i = 0; i < mLevelHeight; ++i)
		{
			delete[] mLevelData[i];
		}
		delete[] mLevelData;
		mLevelData = nullptr;
	}

	// Delete UI screens
    for (auto ui : mUIStack) {
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

Vector2 Game::GetMouseWorldPosition()
{
	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);

	float scaleX = static_cast<float>(GameConstants::WINDOW_WIDTH) / static_cast<float>(mRenderer->GetScreenWidth());
	float scaleY = static_cast<float>(GameConstants::WINDOW_HEIGHT) / static_cast<float>(mRenderer->GetScreenHeight());
	mouseX = static_cast<int>(mouseX * scaleX);
	mouseY = static_cast<int>(mouseY * scaleY);

	Vector2 worldPos;
	worldPos.x = static_cast<float>(mouseX) + mCameraPos.x;
	worldPos.y = static_cast<float>(mouseY) + mCameraPos.y;

	return worldPos;
}