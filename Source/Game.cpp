#include <algorithm>
#include <vector>
#include <map>
#include <fstream>
#include "Actors/Characters/BossBase.h"
#include "Actors/Characters/Dummy.h"
#include "Actors/Characters/Enemies/WhiteBoss.h"
#include "Actors/Characters/Enemies/WhiteCat.h"
#include "Actors/Characters/EnemyBase.h"
#include "Actors/UpgradeTreat.h"
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
#include "UI/Screens/UpgradeHUD.h"
#include "UI/Screens/GameOver.h"
#include "UI/Screens/WinScreen.h"
#include "Actors/Actor.h"
#include "Actors/Block.h"
#include "Actors/Spawner.h"
#include "Actors/Characters/ShadowCat.h"
#include "Components/AnimatedParticleSystemComponent.h"
#include "Actors/Characters/Boss.h"
#include "Actors/LevelPortal.h"

Game::Game()
	: mWindow(nullptr),
	  mRenderer(nullptr),
	  mTicksCount(0),
	  mIsRunning(true),
	  mIsDebugging(false),
	  mIsGodMode(true),
	  mUpdatingActors(false),
	  mCameraPos(Vector2::Zero),
	  mLevelData(nullptr),
	  mAudio(nullptr),
	  mHUD(nullptr),
	  mTutorialHUD(nullptr),
	  mUpgradeHUD(nullptr),
	  mLevelPortal(nullptr),
	  mCurrentBoss(nullptr),
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

	// Hide cursor, we use our own
	SDL_ShowCursor(SDL_DISABLE);

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
	// Stop background music
	if (mBackgroundMusic.IsValid())
	{
		mAudio->StopSound(mBackgroundMusic);
		mBackgroundMusic.Reset();
	}

	// Use state so we can call this from within an actor update
	for (auto *actor : mActors)
	{
		if (actor->IsPersistent()) continue;
		actor->SetState(ActorState::Destroy);
	}

	// delete actors before clearing lists
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

	mStompActors.clear();
	mFurBallActors.clear();
	mEnemies.clear();

	// Delete UI screens
	for (auto ui : mUIStack)
	{
		// Don't delete HUD, Tutorial HUD, or Upgrade HUD here, they persist between scenes
		if (ui == mHUD || ui == mTutorialHUD || ui == mUpgradeHUD)
			continue;

		delete ui;
	}
	mUIStack.clear();
	if (mTutorialHUD)
		mUIStack.push_back(mTutorialHUD);
	if (mHUD)
		mUIStack.push_back(mHUD);
	if (mUpgradeHUD)
		mUIStack.push_back(mUpgradeHUD);

	// Reset states
	mLevelPortal = nullptr;
	mCurrentBoss = nullptr;
}

void Game::PauseGame()
{
	// Pause all actors
	mIsPaused = true;

	for (auto *actor : mActors)
		actor->SetState(ActorState::Paused);
}

void Game::ResumeGame()
{
	mIsPaused = false;

	for (auto *actor : mActors)
		actor->SetState(ActorState::Active);
}

void Game::ResetGame()
{
	// kill ui screens except huds
	auto iter = mUIStack.begin();
	while (iter != mUIStack.end())
	{
		if ((*iter)->GetState() == UIScreen::UIState::Closing)
		{
			delete *iter;
			iter = mUIStack.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	// reset game over conditions
	SetGameOver(false);
	SetGameWon(false);

	SetScene(GameScene::Level1);

	mCurrentBoss = nullptr;

	mShadowCat->SetHP(mShadowCat->GetMaxHP());

	ResumeGame();
}

void Game::SetScene(GameScene nextScene)
{
	UnloadScene();

	switch (nextScene)
	{
	case GameScene::MainMenu:
		mCurrentScene = GameScene::MainMenu;

		// Main menu background music
		mBackgroundMusic = mAudio->PlaySound("m01_main_menu.mp3", true, 0.3f);

		new MainMenu(this, "../Assets/Fonts/Pixellari.ttf");
		break;

	case GameScene::Lobby:
		mCurrentScene = GameScene::Lobby;

		// Lobby background music
		mBackgroundMusic = mAudio->PlaySound("m04_tutorial.mp3", true, 0.5f);

		// Toggleable tutorial HUD
		if (!mTutorialHUD)
			mTutorialHUD = new TutorialHUD(this, "../Assets/Fonts/Pixellari.ttf");

		// Always shown
		mHUD = new HUD(this, "../Assets/Fonts/Pixellari.ttf");
		mUpgradeHUD = new UpgradeHUD(this, "../Assets/Fonts/Pixellari.ttf");

		// Show tutorial in Lobby
		if (mTutorialHUD)
			mTutorialHUD->ShowControls();

		InitializeActors();
		break;

	case GameScene::Level1:
		mCurrentScene = GameScene::Level1;

		// Level 1 background music
		mBackgroundMusic = mAudio->PlaySound("m05_level1.mp3", true, 0.5f);

		// Hide tutorial when entering levels
		if (mTutorialHUD)
			mTutorialHUD->HideControls();

		InitializeActors();
		break;

	case GameScene::Level1_Boss:
		mCurrentScene = GameScene::Level1_Boss;

		// Boss 1 background music
		mBackgroundMusic = mAudio->PlaySound("m08_boss1_grass.mp3", true, 0.3f);

		// Hide tutorial when entering levels
		if (mTutorialHUD)
			mTutorialHUD->HideControls();

		InitializeActors();
		break;

	case GameScene::Level2:
		mCurrentScene = GameScene::Level2;

		// Level 2 background music
		mBackgroundMusic = mAudio->PlaySound("m06_level2.mp3", true, 0.6f);

		// Hide tutorial when entering levels
		if (mTutorialHUD)
			mTutorialHUD->HideControls();

		InitializeActors();
		break;

	case GameScene::Level2_Boss:
		mCurrentScene = GameScene::Level2_Boss;

		// Boss 2 background music
		mBackgroundMusic = mAudio->PlaySound("m09_boss2_bricks.mp3", true);

		// Hide tutorial when entering levels
		if (mTutorialHUD)
			mTutorialHUD->HideControls();

		InitializeActors();
		break;

	case GameScene::Level3:
		mCurrentScene = GameScene::Level3;

		// Level 3 background music
		mBackgroundMusic = mAudio->PlaySound("m07_level3.mp3", true);

		// Hide tutorial when entering levels
		if (mTutorialHUD)
			mTutorialHUD->HideControls();

		InitializeActors();
		break;

	case GameScene::Level3_Boss:
		mCurrentScene = GameScene::Level3_Boss;

		// Boss 3 background music
		mBackgroundMusic = mAudio->PlaySound("m10_boss3_stone.mp3", true);

		// Hide tutorial when entering levels
		if (mTutorialHUD)
			mTutorialHUD->HideControls();

		InitializeActors();
		break;
	}
}

GroundType Game::GetGroundType() const
{
	switch (mCurrentScene)
	{
	case GameScene::Lobby:
	case GameScene::Level1:
	case GameScene::Level1_Boss:
		return GroundType::Grass;
	
	case GameScene::Level2:
	case GameScene::Level2_Boss:
		return GroundType::Brick;
	
	case GameScene::Level3:
	case GameScene::Level3_Boss:
		return GroundType::Stone;
	
	default:
		return GroundType::Grass;
	}
}

void Game::InitializeActors()
{
	mCollisionQueryActor = new Actor(this);
	new ColliderComponent(mCollisionQueryActor, 0, 0, nullptr, CollisionFilter());

	// Initialize debug actor
	mDebugActor = new DebugActor(this);

	mAttackTrailActor = new Actor(this);
	new AnimatedParticleSystemComponent(mAttackTrailActor, "AttackTrailAnim", false);

	std::string levelPath;

	// Choose level based on current scene
	if (mCurrentScene == GameScene::Lobby)
	{
		levelPath = "../Assets/Levels/Lobby/Lobby.csv";
	}
	else if (mCurrentScene == GameScene::Level1)
	{
		levelPath = "../Assets/Levels/Level1/Level1.csv";
	}
	else if (mCurrentScene == GameScene::Level1_Boss)
	{
		levelPath = "../Assets/Levels/Level1_Boss/Level1_Boss.csv";
	}
	else if (mCurrentScene == GameScene::Level2)
	{
		levelPath = "../Assets/Levels/Level2/Level2.csv";
	}
	else if (mCurrentScene == GameScene::Level2_Boss)
	{
		levelPath = "../Assets/Levels/Level2_Boss/Level2_Boss.csv";
	}
	else if (mCurrentScene == GameScene::Level3)
	{
		levelPath = "../Assets/Levels/Level3/Level3.csv";
	}
	else if (mCurrentScene == GameScene::Level3_Boss)
	{
		levelPath = "../Assets/Levels/Level3_Boss/Level3_Boss.csv";
	}
	else
	{
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
				if (!mShadowCat) mShadowCat = new ShadowCat(this, position);
				else mShadowCat->SetPosition(position);
			}
			else if (tileID == 1)
			{
			}
			else if (tileID == 2)
			{
				auto whiteCat = new WhiteCat(this, position);
			}
			else if (tileID == 3)
			{
			}
			// Blocks (excluding tile 9 - carpet/portal, handled by LevelPortal actor)
			else if (tileID >= 4 && tileID <= 10)
			{
				if (tileID == 9)
				{
					SDL_Log("[BUILD] Skipping tile 9 (carpet) at (%.1f, %.1f) - will be rendered by LevelPortal", position.x, position.y);
				}
				else
				{
					auto block = new Block(this, tileID);
					block->SetPosition(position);
				}
			}
			// Dummy
			else if (tileID == 11)
			{
				auto dummy = new Dummy(this, position);
			}
			// Walls (paredes)
			else if (tileID >= 16 && tileID <= 27)
			{
				auto block = new Block(this, tileID);
				block->SetPosition(position);
			}
			else if (tileID == 12)
			{
				auto boss = new WhiteBoss(this, position);
			}
			else if (tileID == 13)
			{
			}
			else if (tileID == 14)
			{
			}
			else if (tileID == 15)
			{
			}
		}
	}

	// Spawn portal at center of last row (initially hidden)
	int centerColumn = width / 2;
	int lastRow = height - 1;
	float portalX = (centerColumn * GameConstants::TILE_SIZE) + (GameConstants::TILE_SIZE / 2.0f);
	float portalY = (lastRow * GameConstants::TILE_SIZE) + (GameConstants::TILE_SIZE / 2.0f);

	mLevelPortal = new LevelPortal(this);
	mLevelPortal->SetPosition(Vector2(portalX, portalY));

	// Show portal immediately only in debug mode
	// For all levels (including Lobby), portal will activate via UpdateGame logic
	if (mIsDebugging)
	{
		SDL_Log("[BUILD] Debug mode enabled - showing portal immediately");
		mLevelPortal->Activate();
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
			if (!mUIStack.empty())
			{
				mUIStack.back()->HandleKeyPress(event.key.keysym.sym);
			}

			if (event.key.keysym.sym == SDLK_n && event.key.repeat == 0)
			{
				SetScene(GameScene::Level1);
			}

			// Fullscreen toggle
			if (event.key.keysym.sym == SDLK_F11 && event.key.repeat == 0)
			{
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

			// God Mode toggle
			if (event.key.keysym.sym == SDLK_F2 && event.key.repeat == 0)
			{
				mIsGodMode = !mIsGodMode;
				SDL_Log("[GOD MODE] %s", mIsGodMode ? "ENABLED" : "DISABLED");
			}

			// Tutorial HUD toggle
			if (event.key.keysym.sym == SDLK_h && event.key.repeat == 0)
				if (mTutorialHUD)
					mTutorialHUD->ToggleControlVisibility(); // Pass event to actors

			// Pause toggle
			if (event.key.keysym.sym == SDLK_ESCAPE && event.key.repeat == 0)
				if (mCurrentScene > GameScene::MainMenu && mShadowCat && mShadowCat->GetUpgradePoints() == 0) {
					if (mIsPaused)
						ResumeGame();
					else
						PauseGame();
				}

			for (auto actor : mActors)
				actor->OnHandleEvent(event);
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
	if (mIsGameOver || mIsGameWon)
	{
		PauseGame();

		if (mIsGameOver)
		{
			new GameOver(this, "../Assets/Fonts/Pixellari.ttf");
		}
		else
		{
			new WinScreen(this, "../Assets/Fonts/Pixellari.ttf");
		}
	}

	// Update all actors and pending actors
	UpdateActors(deltaTime);

	// Update camera position
	UpdateCamera();

	// ========== PORTAL ACTIVATION ==========
	// Activate portal when all enemies and bosses are defeated (or in God Mode)
	if (mShadowCat && mLevelPortal)
	{
		int aliveEnemies = CountAliveEnemies();
		int aliveBosses = CountAliveBosses();

		// Activate portal when all threats are cleared OR in God Mode
		if ((aliveEnemies == 0 && aliveBosses == 0 || mIsGodMode) && !mLevelPortal->IsActive())
		{
			if (mIsGodMode)
			{
				SDL_Log("[PORTAL] ACTIVATING - God Mode enabled (Enemies: %d, Bosses: %d)",
						aliveEnemies, aliveBosses);
			}
			else
			{
				SDL_Log("[PORTAL] ACTIVATING - All threats cleared! (Enemies: %d, Bosses: %d)",
						aliveEnemies, aliveBosses);
			}
			mLevelPortal->Activate();
		}
	}

	// ========== LEVEL TRANSITION (grid-based, but requires portal to be active) ==========
	if (mShadowCat && mLevelPortal && mLevelPortal->IsActive())
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
				SDL_Log("[TRANSITION] Portal active - Transitioning: Lobby -> Level 1");
				break;
			case GameScene::Level1:
				nextScene = GameScene::Level1_Boss;
				SDL_Log("Transitioning: Level 1 -> Level 1 Boss");
				break;
			case GameScene::Level1_Boss:
				nextScene = GameScene::Level2;
				SDL_Log("Transitioning: Level 1 Boss -> Level 2");
				break;
			case GameScene::Level2:
				nextScene = GameScene::Level2_Boss;
				SDL_Log("Transitioning: Level 2 -> Level 2 Boss");
				break;
			case GameScene::Level2_Boss:
				nextScene = GameScene::Level3;
				SDL_Log("Transitioning: Level 2 Boss -> Level 3");
				break;
			case GameScene::Level3:
				nextScene = GameScene::Level3_Boss;
				SDL_Log("Transitioning: Level 3 -> Level 3 Boss");
				break;
			case GameScene::Level3_Boss:
				SetGameWon(true);
				break;
			default:
				break;
			}

			if (!mIsGameWon)
			{
				// Play room change sound
				mAudio->PlaySound("e16_change_room.mp3", false, 0.3f);
				SetScene(nextScene);
			}
		}
	}

	// Audio and UI
	if (mAudio)
		mAudio->Update(deltaTime);

	// Update UI screens
	for (auto ui : mUIStack)
	{
		if (ui->GetState() == UIScreen::UIState::Active)
		{
			ui->Update(deltaTime);
		}
	}

	// Delete any UI that are closed
	auto iter = mUIStack.begin();
	while (iter != mUIStack.end())
	{
		if ((*iter)->GetState() == UIScreen::UIState::Closing)
		{
			delete *iter;
			iter = mUIStack.erase(iter);
		}
		else
		{
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

void Game::AddPersistentActor(Actor *actor)
{
	mPersistentActors.emplace_back(actor);
}

void Game::RemovePersistentActor(Actor *actor)
{
	auto iter = std::find(mPersistentActors.begin(), mPersistentActors.end(), actor);
	if (iter != mPersistentActors.end())
	{
		std::iter_swap(iter, mPersistentActors.end() - 1);
		mPersistentActors.pop_back();
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

	switch (mCurrentScene)
	{
	case GameScene::MainMenu:
		backgroundPath = "../Assets/HUD/Background/MainMenuBackground.png";
		break;
	case GameScene::Lobby:
		backgroundPath = "../Assets/Levels/Lobby/LobbyBackground.png";
		break;
	case GameScene::Level1:
		backgroundPath = "../Assets/Levels/Level1/Level1Background.png";
		break;
	case GameScene::Level1_Boss:
		backgroundPath = "../Assets/Levels/Level1_Boss/Level1_Boss_Background.png";
		break;
	case GameScene::Level2:
		backgroundPath = "../Assets/Levels/Level2/Level2Background.png";
		break;
	case GameScene::Level2_Boss:
		backgroundPath = "../Assets/Levels/Level2_Boss/Level2_Boss_Background.png";
		break;
	case GameScene::Level3:
		backgroundPath = "../Assets/Levels/Level3/Level3Background.png";
		break;
	case GameScene::Level3_Boss:
		backgroundPath = "../Assets/Levels/Level3_Boss/Level3_Boss_Background.png";
		break;
	default:
		backgroundPath = "../Assets/Levels/Lobby/LobbyBackground.png";
		break;
	}

	Texture *backgroundTexture = mRenderer->GetTexture(backgroundPath);
	if (backgroundTexture)
	{
		// Main menu static image overrides scaling
		if (mCurrentScene == GameScene::MainMenu)
		{
			Vector2 position(GameConstants::WINDOW_WIDTH / 2.0f, GameConstants::WINDOW_HEIGHT / 2.0f);
			Vector2 size(static_cast<float>(backgroundTexture->GetWidth()), static_cast<float>(backgroundTexture->GetHeight()));

			mRenderer->DrawTexture(position, size, 0.0f, Vector3(1.0f, 1.0f, 1.0f),
								   backgroundTexture, Vector4::UnitRect, Vector2::Zero);
		}
		else
		{
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

		auto actor = drawable->GetOwner();
		if (mIsDebugging)
		{
			// Call debug draw for actor
			auto actor = drawable->GetOwner();

			// Check if actor is a Boss and call its debug draw
			auto boss = dynamic_cast<Boss *>(actor);
			if (boss)
			{
				boss->OnDebugDraw(mRenderer);
			}

			// Call debug draw for actor components
			for (auto comp : actor->GetComponents())
			{
				comp->DebugDraw(mRenderer);
			}
		}

		for (auto comp : actor->GetComponents())
			comp->ComponentDraw(mRenderer);
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
	for (auto ui : mUIStack)
	{
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

Vector2 Game::GetMouseAbsolutePosition()
{
	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);

	float scaleX = static_cast<float>(GameConstants::WINDOW_WIDTH) / static_cast<float>(mRenderer->GetScreenWidth());
	float scaleY = static_cast<float>(GameConstants::WINDOW_HEIGHT) / static_cast<float>(mRenderer->GetScreenHeight());
	mouseX = static_cast<int>(mouseX * scaleX);
	mouseY = static_cast<int>(mouseY * scaleY);

	Vector2 mousePos = Vector2(static_cast<float>(mouseX), static_cast<float>(mouseY));
	return mousePos;
}

StompActor *Game::GetStompActor()
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

FurBallActor *Game::GetFurBallActor()
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

UpgradeTreat *Game::GetUpgradeTreatActor()
{
	UpgradeTreat *treat = nullptr;
	for (auto actor : mUpgradeTreatActors)
		if (actor->IsCollected())
			treat = actor;
	if (!treat)
	{
		treat = new UpgradeTreat(this);
		mUpgradeTreatActors.push_back(treat);
	}
	return treat;
}

void Game::RegisterEnemy(EnemyBase *enemy)
{
	if (enemy)
		mEnemies.push_back(enemy);
}

void Game::RegisterBoss(BossBase *boss)
{
	if (!boss) return;

	mCurrentBoss = boss;
}

void Game::UnregisterEnemy(EnemyBase *enemy)
{
	auto iter = std::find(mEnemies.begin(), mEnemies.end(), enemy);
	if (iter != mEnemies.end())
		mEnemies.erase(iter);
}

void Game::UnregisterBoss(BossBase *boss)
{
	if (mCurrentBoss != boss) return;
	
	mCurrentBoss = nullptr;
}

int Game::CountAliveEnemies() const
{
	int count = 0;
	for (auto enemy : mEnemies)
	{
		if (enemy && enemy->GetState() == ActorState::Active && !enemy->IsDead())
			count++;
	}
	return count;
}

int Game::CountAliveBosses() const
{
	int count = 0;
	if (mCurrentBoss) count += !mCurrentBoss->IsDead();
	return count;
}