#include <algorithm>
#include <vector>
#include <map>
#include <fstream>
#include "Actors/Characters/Dummy.h"
#include "CSV.h"
#include "Game.h"
#include "Components/Skills/Stomp.h"
#include "GameConstants.h"
#include "Components/Drawing/DrawComponent.h"
#include "Components/Physics/RigidBodyComponent.h"
#include "Random.h"
#include "UI/Screens/MainMenu.h"
#include "UI/Screens/HUD.h"
#include "UI/Screens/TutorialHUD.h"
#include "UI/Screens/GameOver.h"
#include "UI/Screens/WinScreen.h"
#include "Actors/Actor.h"
#include "Actors/Block.h"
#include "Actors/Spawner.h"
#include "Actors/Characters/ShadowCat.h"
#include "Components/AnimatedParticleSystemComponent.h"
#include "Actors/Characters/Enemy.h"

Game::Game()
	: mWindow(nullptr),
	mRenderer(nullptr),
	mTicksCount(0),
	mIsRunning(true),
	mIsDebugging(false),
	mUpdatingActors(false),
	mCameraPos(Vector2::Zero),
	mLevelData(nullptr),
	mAudio(nullptr),
    mHUD(nullptr),
	mTutorialHUD(nullptr),
	mIsPaused(false),
	mIsGameOver(false),
	mIsGameWon(false),
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
    SetScene(GameScene::MainMenu);

	mTicksCount = SDL_GetTicks();

	return true;
}

void Game::UnloadScene()
{
    // Use state so we can call this from within an actor update
    for(auto *actor : mActors) {
        actor->SetState(ActorState::Destroy);
    }

	mStompActors.clear();
	mFurBallActors.clear();

    // Delete UI screens
    for (auto ui : mUIStack) {
		// Don't delete HUD or Tutorial HUD here, they persist between scenes
		if (ui == mHUD || ui == mTutorialHUD) continue;

        delete ui;
    }
    mUIStack.clear();

    // Reset states
	mShadowCat = nullptr;
}

void Game::PauseGame() {
	// Pause all actors
	mIsPaused = true;

	for (auto *actor : mActors)
		actor->SetState(ActorState::Paused);
}

void Game::ResumeGame() {
	mIsPaused = false;

	for (auto *actor : mActors)
		actor->SetState(ActorState::Active);
}

void Game::ResetGame() {
	// Bugged so return for now
	return;
}

void Game::SetScene(GameScene nextScene)
{
    UnloadScene();

    switch (nextScene) {
        case GameScene::MainMenu:
            mCurrentScene = GameScene::MainMenu;
			
			// Main menu back music
			// mAudio->PlaySound("Music.ogg", true);

			new MainMenu(this, "../Assets/Fonts/Pixellari.ttf");
            break;

        case GameScene::Lobby:
            mCurrentScene = GameScene::Lobby;

			// Always shown
			mHUD = new HUD(this, "../Assets/Fonts/Pixellari.ttf"); 

			// Toggleable tutorial HUD
			mTutorialHUD = new TutorialHUD(this, "../Assets/Fonts/Pixellari.ttf");

			InitializeActors();

			break;

        case GameScene::Level1:
            mCurrentScene = GameScene::Level1;

            InitializeActors();
            break;

        case GameScene::Level2:
            mCurrentScene = GameScene::Level2;

            InitializeActors();
            break;

        case GameScene::Level3:
            mCurrentScene = GameScene::Level3;

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
	} else if (mCurrentScene == GameScene::Level2) {
		levelPath = "../Assets/Levels/Level2/Level2.csv";
	} else if (mCurrentScene == GameScene::Level3) {
		levelPath = "../Assets/Levels/Level3/Level3.csv";
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
	// Determine enemy type based on current level
	Enemy::EnemyType currentEnemyType = Enemy::EnemyType::WhiteCat;
	if (mCurrentScene == GameScene::Level2)
	{
		currentEnemyType = Enemy::EnemyType::OrangeCat;
	}
	else if (mCurrentScene == GameScene::Level3)
	{
		currentEnemyType = Enemy::EnemyType::SylvesterCat;
	}
	
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
			// Tile ID 1: Spawner - small patrol (100px)
			// Spawns enemy when player camera comes within ~700px of this position
			else if (tileID == 1)
			{
				// Create waypoints 100 pixels to left and right of spawn position
				Vector2 waypointA = position + Vector2(-100.0f, 0.0f);
				Vector2 waypointB = position + Vector2(100.0f, 0.0f);
				auto spawner = new Spawner(this, waypointA, waypointB, currentEnemyType);
				spawner->SetPosition(position);
			}
			// Tile ID 2: Spawner - medium patrol (150px)
			// Spawns enemy when player camera comes within ~700px of this position
			else if (tileID == 2)
			{
				// Create waypoints 150 pixels to left and right of spawn position
				Vector2 waypointA = position + Vector2(-150.0f, 0.0f);
				Vector2 waypointB = position + Vector2(150.0f, 0.0f);
				auto spawner = new Spawner(this, waypointA, waypointB, currentEnemyType);
				spawner->SetPosition(position);
			}
			// Tile ID 3: Spawner - large patrol (200px)
			// Spawns enemy when player camera comes within ~700px of this position
			else if (tileID == 3)
			{
				// Create waypoints 200 pixels to left and right of spawn position
				Vector2 waypointA = position + Vector2(-200.0f, 0.0f);
				Vector2 waypointB = position + Vector2(200.0f, 0.0f);
				auto spawner = new Spawner(this, waypointA, waypointB, currentEnemyType);
				spawner->SetPosition(position);
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
			// ========== IMMEDIATE ENEMY SPAWNS (spawn when level loads) ==========
			// Tile ID 12: Enemy - small patrol (100px)
			else if (tileID == 12)
			{
				// Create waypoints 100 pixels to left and right of spawn position
				Vector2 waypointA = position + Vector2(-100.0f, 0.0f);
				Vector2 waypointB = position + Vector2(100.0f, 0.0f);
				auto enemy = new Enemy(this, waypointA, waypointB, currentEnemyType);
				enemy->SetPosition(position);
			}
			// Tile ID 13: Enemy with larger patrol (200px)
			else if (tileID == 13)
			{
				// Create waypoints 200 pixels to left and right of spawn position
				Vector2 waypointA = position + Vector2(-200.0f, 0.0f);
				Vector2 waypointB = position + Vector2(200.0f, 0.0f);
				auto enemy = new Enemy(this, waypointA, waypointB, currentEnemyType);
				enemy->SetPosition(position);
			}
			// ========== DELAYED SPAWNERS (spawn when player approaches) ==========
			// Tile ID 14: Spawner - small patrol (100px)
			// Spawns enemy when player camera comes within ~700px of this position
			else if (tileID == 14)
			{
				// Create waypoints 100 pixels to left and right of spawn position
				Vector2 waypointA = position + Vector2(-100.0f, 0.0f);
				Vector2 waypointB = position + Vector2(100.0f, 0.0f);
				auto spawner = new Spawner(this, waypointA, waypointB, currentEnemyType);
				spawner->SetPosition(position);
			}
			// Tile ID 15: Spawner - large patrol (200px)
			// Spawns enemy when player camera comes within ~700px of this position
			else if (tileID == 15)
			{
				// Create waypoints 200 pixels to left and right of spawn position
				Vector2 waypointA = position + Vector2(-200.0f, 0.0f);
				Vector2 waypointB = position + Vector2(200.0f, 0.0f);
				auto spawner = new Spawner(this, waypointA, waypointB, currentEnemyType);
				spawner->SetPosition(position);
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
		case SDL_MOUSEBUTTONDOWN:
			// Handle key press for UI screens
			if (!mUIStack.empty()) {
				mUIStack.back()->HandleKeyPress(event.key.keysym.sym);
			}

			if (event.key.keysym.sym == SDLK_n && event.key.repeat == 0) {
				SetScene(GameScene::Level1);
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

			// Tutorial HUD toggle
			if (event.key.keysym.sym == SDLK_h && event.key.repeat == 0)
				if (mTutorialHUD)
					mTutorialHUD->ToggleControlVisibility();

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
	// End conditions check
	if (mIsGameOver || mIsGameWon) {
		PauseGame();

		if (mIsGameOver) {
			new GameOver(this, "../Assets/Fonts/Pixellari.ttf");
		} else {
			new WinScreen(this, "../Assets/Fonts/Pixellari.ttf");
		}
	}

	// Update all actors and pending actors
	UpdateActors(deltaTime);

	// Update camera position
	UpdateCamera();

	if (mShadowCat)
	{
		Vector2 playerPos = mShadowCat->GetPosition();
		int gridX = static_cast<int>(playerPos.x / GameConstants::TILE_SIZE);
		int gridY = static_cast<int>(playerPos.y / GameConstants::TILE_SIZE);

		int centerColumn = mLevelWidth / 2;
		int leftColumn = centerColumn - 1;
		int rightColumn = centerColumn + 1;

		int lastRow = mLevelHeight - 1;
		bool isInCentralColumns = (gridX == leftColumn || gridX == centerColumn || gridX == rightColumn);
		bool isInLastRow = (gridY == lastRow);

		if (isInLastRow && isInCentralColumns)
		{
			GameScene nextScene = GameScene::Lobby;

			switch (mCurrentScene)
			{
			case GameScene::Lobby:
				nextScene = GameScene::Level1;
				SDL_Log("Transitioning: Lobby -> Level 1");
				break;
			case GameScene::Level1:
				nextScene = GameScene::Level2;
				SDL_Log("Transitioning: Level 1 -> Level 2");
				break;
			case GameScene::Level2:
				nextScene = GameScene::Level3;
				SDL_Log("Transitioning: Level 2 -> Level 3");
				break;
			case GameScene::Level3:
				SetGameWon(true);
				break;
				
				break;
			default:
				return;
			}

			if (!mIsGameWon)
				SetScene(nextScene);
		}
	}

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

		// Since lobby is small, we allow camera to go out of bounds
		if (mCurrentScene == GameScene::Lobby)
			return;
		
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

void Game::AddCollider(class ColliderComponent *collider)
{
	mColliders.emplace_back(collider);
}

void Game::RemoveCollider(ColliderComponent *collider)
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

	switch (mCurrentScene) {
		case GameScene::MainMenu:
			backgroundPath = "../Assets/HUD/MainMenuBackground.png";
			break;
		case GameScene::Lobby:
			backgroundPath = "../Assets/Levels/Lobby/LobbyBackground.png";
			break;
		case GameScene::Level1:
			backgroundPath = "../Assets/Levels/Level1/Level1Background.png";
			break;
		case GameScene::Level2:
			backgroundPath = "../Assets/Levels/Level2/Level2Background.png";
			break;
		case GameScene::Level3:
			backgroundPath = "../Assets/Levels/Level3/Level3Background.png";
			break;
		default:
			backgroundPath = "../Assets/Levels/Lobby/LobbyBackground.png";
			break;
	}

	Texture *backgroundTexture = mRenderer->GetTexture(backgroundPath);
	if (backgroundTexture)
	{
		// Main menu static image overrides scaling
		if (mCurrentScene == GameScene::MainMenu) {
			Vector2 position(GameConstants::WINDOW_WIDTH / 2.0f, GameConstants::WINDOW_HEIGHT / 2.0f);
			Vector2 size(static_cast<float>(backgroundTexture->GetWidth()), static_cast<float>(backgroundTexture->GetHeight()));

			mRenderer->DrawTexture(position, size, 0.0f, Vector3(1.0f, 1.0f, 1.0f),
								   backgroundTexture, Vector4::UnitRect, Vector2::Zero);
		} else {
		// Align background to level size	
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
	}

	for (auto drawable : mDrawables)
	{
		drawable->Draw(mRenderer);

		if (mIsDebugging)
		{
			// Call debug draw for actor
			auto actor = drawable->GetOwner();
			
			// Check if actor is an Enemy and call its debug draw
			auto enemy = dynamic_cast<Enemy*>(actor);
			if (enemy)
			{
				enemy->OnDebugDraw(mRenderer);
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

StompActor* Game::GetStompActor()
{
	StompActor *stomp = nullptr;
	for (auto actor : mStompActors)
		if (actor->IsDead())
			stomp = actor;

	if (!stomp)
	{
		stomp = new StompActor(this);
		mStompActors.push_back(stomp);
	}
	
	return stomp;
}

FurBallActor* Game::GetFurBallActor()
{
	FurBallActor *furball = nullptr;
	for (auto actor : mFurBallActors)
		if (actor->IsDead())
			furball = actor;

	if (!furball)
	{
		furball = new FurBallActor(this);
		mFurBallActors.push_back(furball);
	}
	
	return furball;
}