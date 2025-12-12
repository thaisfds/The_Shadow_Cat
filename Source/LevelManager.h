#pragma once
#include <string>
#include <vector>
#include "Math.h"
#include "GameConstants.h"

// Forward declarations
class Game;
class Actor;
class DebugActor;
class LevelPortal;
class ShadowCat;
class EnemyBase;
class BossBase;
class StompActor;
class FurBallActor;
class WhiteBombActor;
class WhiteBubbleActor;
class UpgradeTreat;
class DrawComponent;
class ColliderComponent;

enum class GameScene;
enum class ActorState;

class LevelManager
{
public:
    // Singleton access
    static LevelManager& Instance();
    
    // Initialize with game reference
    void Initialize(Game* game);
    void Shutdown();

    // Delete copy constructor and assignment operator
    LevelManager(const LevelManager&) = delete;
    LevelManager& operator=(const LevelManager&) = delete;

    // Level loading and management
    void LoadAndBuildLevel(GameScene scene);
    void UnloadLevel();

    // Update and input
    void ProcessInput(const Uint8* keyState);
    void Update(float deltaTime);
    void UpdateCamera(float deltaTime);

    // Actor management
    void AddActor(Actor* actor);
    void RemoveActor(Actor* actor);
    void AddPersistentActor(Actor* actor);
    void RemovePersistentActor(Actor* actor);

    // Draw component management
    void AddDrawable(DrawComponent* drawable);
    void RemoveDrawable(DrawComponent* drawable);
    std::vector<DrawComponent*>& GetDrawables() { return mDrawables; }

    // Collider management
    void AddCollider(ColliderComponent* collider);
    void RemoveCollider(ColliderComponent* collider);
    std::vector<ColliderComponent*>& GetColliders() { return mColliders; }

    // Enemy/Boss tracking
    void RegisterEnemy(EnemyBase* enemy);
    void RegisterBoss(BossBase* boss);
    void UnregisterEnemy(EnemyBase* enemy);
    void UnregisterBoss(BossBase* boss);
    int CountAliveEnemies() const;
    int CountAliveBosses() const;

    // Actor pools
    StompActor* GetStompActor();
    FurBallActor* GetFurBallActor();
    WhiteBombActor* GetWhiteBombActor();
    WhiteBubbleActor* GetWhiteBubbleActor();
    UpgradeTreat* GetUpgradeTreatActor();

    // Getters
    int GetLevelWidth() const { return mLevelWidth; }
    int GetLevelHeight() const { return mLevelHeight; }
    LevelPortal* GetLevelPortal() const { return mLevelPortal; }
    ShadowCat* GetPlayer() const { return mShadowCat; }
    BossBase* GetCurrentBoss() const { return mCurrentBoss; }
    Actor* GetCollisionQueryActor() const { return mCollisionQueryActor; }
    DebugActor* GetDebugActor() const { return mDebugActor; }
    Actor* GetAttackTrailActor() const { return mAttackTrailActor; }
    Actor* GetWhiteSlashActor() const { return mWhiteSlashActor; }
    Vector2& GetCameraPos() { return mCameraPos; }
    std::vector<Actor*>& GetActors() { return mActors; }
    
    // Setters
    void SetCameraPos(const Vector2& position) { mCameraPos = position; }

private:
    // Private constructor for singleton
    LevelManager();
    ~LevelManager();

    // Helper methods
    int** LoadLevelFromFile(const std::string& fileName, int& outWidth, int& outHeight);
    void BuildLevel(int** levelData, int width, int height);
    void SpawnActorByTileID(int tileID, const Vector2& position);
    std::string GetLevelPath(GameScene scene) const;
    void UpdateActors(float deltaTime);
    void UpdatePortalActivation();
    bool CheckLevelTransition(const Vector2& playerPos, GameScene& outNextScene);

    Game* mGame;

    // Level data
    int** mLevelData;
    int mLevelWidth;
    int mLevelHeight;
    GameScene mCurrentScene;

    // Camera
    Vector2 mCameraPos;

    // Core actors
    ShadowCat* mShadowCat;
    LevelPortal* mLevelPortal;
    Actor* mCollisionQueryActor;
    DebugActor* mDebugActor;
    Actor* mAttackTrailActor;
    Actor* mWhiteSlashActor;

    // Actor lists
    std::vector<Actor*> mActors;
    std::vector<Actor*> mPendingActors;
    std::vector<Actor*> mPersistentActors;
    bool mUpdatingActors;

    // Draw and collision
    std::vector<DrawComponent*> mDrawables;
    std::vector<ColliderComponent*> mColliders;

    // Enemy and Boss tracking
    std::vector<EnemyBase*> mEnemies;
    BossBase* mCurrentBoss;

    // Actor pools
    std::vector<StompActor*> mStompActors;
    std::vector<FurBallActor*> mFurBallActors;
    std::vector<WhiteBombActor*> mWhiteBombActors;
    std::vector<WhiteBubbleActor*> mWhiteBubbleActors;
    std::vector<UpgradeTreat*> mUpgradeTreatActors;
};