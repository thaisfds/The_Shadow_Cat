#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>
#include "Actors/Actor.h"
#include "Actors/Characters/BossBase.h"
#include "Actors/Characters/EnemyBase.h"
#include "Actors/DebugActor.h"
#include "Actors/Characters/Boss.h"
#include "Renderer/Renderer.h"
#include "AudioSystem.h"
#include "Components/Skills/FurBall.h"
#include "Components/Skills/Stomp.h"

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
	void PushUI(class UIScreen *screen) { mUIStack.emplace_back(screen); }
	const std::vector<class UIScreen *> &GetUIStack() { return mUIStack; }

	// Audio
	AudioSystem *GetAudio() { return mAudio; }

	// Scene Handling
	void SetScene(GameScene scene);
	void UnloadScene();

	// Pause Handling
	void PauseGame();
	void ResumeGame();
	void ResetGame();

	void SetGameOver(bool isOver) { mIsGameOver = isOver; }
	void SetGameWon(bool isWon) { mIsGameWon = isWon; }

	// Renderer
	class Renderer *GetRenderer() { return mRenderer; }

	// Draw functions
	void AddDrawable(class DrawComponent *drawable);
	void RemoveDrawable(class DrawComponent *drawable);
	std::vector<class DrawComponent *> &GetDrawables() { return mDrawables; }

	// Collider functions
	void AddCollider(class ColliderComponent *collider);
	void RemoveCollider(class ColliderComponent *collider);
	std::vector<class ColliderComponent *> &GetColliders() { return mColliders; }

	// Camera functions
	Vector2 &GetCameraPos() { return mCameraPos; };
	void SetCameraPos(const Vector2 &position) { mCameraPos = position; };

	// Mouse position
	Vector2 GetMouseWorldPosition();
	Vector2 GetMouseAbsolutePosition();

	// Game specific
	const class ShadowCat *GetPlayer() { return mShadowCat; }
	class BossBase *GetCurrentBoss() const { return mCurrentBoss; }
	class HUD *GetHUD() { return mHUD; }

	// Enemy and Boss tracking
	void RegisterEnemy(class EnemyBase *enemy);
	void RegisterBoss(class BossBase *boss);
	void UnregisterEnemy(class EnemyBase *enemy);
	void UnregisterBoss(class BossBase *boss);
	int CountAliveEnemies() const;
	int CountAliveBosses() const;

	// Level dimensions
	int GetLevelWidth() const { return mLevelWidth; }
	int GetLevelHeight() const { return mLevelHeight; }

	SDL_GameController *mController;

	// Debug
	bool IsDebugging() { return mIsDebugging; }
	DebugActor *GetDebugActor() { return mDebugActor; }

	Actor *GetCollisionQueryActor() { return mCollisionQueryActor; }
	Actor *GetAttackTrailActor() { return mAttackTrailActor; }
	StompActor *GetStompActor();
	FurBallActor *GetFurBallActor();

private:
	void ProcessInput();
	void UpdateGame(float deltaTime);
	void UpdateCamera();
	void GenerateOutput();

	// Level loading
	int **LoadLevel(const std::string &fileName, int &outWidth, int &outHeight);
	void BuildLevel(int **levelData, int width, int height);

	// Current level dimensions
	int mLevelWidth;
	int mLevelHeight;

	// All the actors in the game
	std::vector<class Actor *> mActors;
	std::vector<class Actor *> mPendingActors;

	// Camera
	Vector2 mCameraPos;

	// All the draw components
	std::vector<class DrawComponent *> mDrawables;

	// All the collision components
	std::vector<class ColliderComponent *> mColliders;

	// All UI screens in the game
	std::vector<class UIScreen *> mUIStack;

	// SDL stuff
	SDL_Window *mWindow;
	class Renderer *mRenderer;

	// Audio system
	AudioSystem *mAudio;

	// Track elapsed time since game start
	Uint32 mTicksCount;

	// Track if we're updating actors right now
	bool mIsRunning;
	bool mUpdatingActors;
	bool mIsFullscreen;
	GameScene mCurrentScene;

	bool mIsPaused;

	// End condition
	bool mIsGameOver;
	bool mIsGameWon;

	// Game-specific
	class ShadowCat *mShadowCat;
	class HUD *mHUD;
	class TutorialHUD *mTutorialHUD;
	class LevelPortal *mLevelPortal;
	int **mLevelData;

	// To use for colliders not attached to a collider component
	class Actor *mCollisionQueryActor;

	class std::vector<StompActor *> mStompActors;
	class std::vector<FurBallActor *> mFurBallActors;

	// Global particle system
	class Actor *mAttackTrailActor;

	// Enemy and Boss tracking
	std::vector<class EnemyBase *> mEnemies;
	class BossBase *mCurrentBoss;  // Pointer to active boss in current room (null when no boss)

	// Boss spawn data (stored until enemies are defeated)
	struct BossSpawnData
	{
		Vector2 arenaCenter;
		Boss::BossType bossType;
		bool playSpawnAnimation;
	};
	std::vector<BossSpawnData> mPendingBossSpawns;

	// Debug
	bool mIsDebugging;
	bool mIsGodMode;
	DebugActor *mDebugActor;
};
