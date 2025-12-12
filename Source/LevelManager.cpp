#include "LevelManager.h"
#include "Game.h"
#include "CSV.h"
#include "GameConstants.h"
#include <fstream>
#include <algorithm>
#include <SDL.h>

// Actor includes
#include "Actors/Actor.h"
#include "Actors/Block.h"
#include "Actors/DebugActor.h"
#include "Actors/LevelPortal.h"
#include "Actors/Characters/ShadowCat.h"
#include "Actors/Characters/Dummy.h"
#include "Actors/Characters/EnemyBase.h"
#include "Actors/Characters/BossBase.h"
#include "Actors/Characters/Enemies/WhiteCat.h"
#include "Actors/Characters/Enemies/OrangeCat.h"
#include "Actors/Characters/Enemies/SylvesterCat.h"
#include "Actors/Characters/Enemies/WhiteBoss.h"
#include "Actors/Characters/Enemies/OrangeBoss.h"
#include "Actors/Characters/Enemies/SylvesterBoss.h"
#include "Components/AnimatedParticleSystemComponent.h"
#include "Components/Physics/ColliderComponent.h"
#include "Components/Drawing/DrawComponent.h"
#include "Components/Skills/Stomp.h"
#include "Components/Skills/FurBall.h"
#include "Components/Skills/WhiteBomb.h"
#include "Components/Skills/WhiteBubble.h"
#include "Actors/UpgradeTreat.h"

LevelManager& LevelManager::Instance()
{
    static LevelManager instance;
    return instance;
}

LevelManager::LevelManager()
    : mGame(nullptr),
      mLevelData(nullptr),
      mLevelWidth(0),
      mLevelHeight(0),
      mCurrentScene(GameScene::MainMenu),
      mCameraPos(Vector2::Zero),
      mShadowCat(nullptr),
      mLevelPortal(nullptr),
      mCollisionQueryActor(nullptr),
      mDebugActor(nullptr),
      mAttackTrailActor(nullptr),
      mWhiteSlashActor(nullptr),
      mUpdatingActors(false),
      mCurrentBoss(nullptr)
{
}

LevelManager::~LevelManager()
{
    Shutdown();
}

void LevelManager::Initialize(Game* game)
{
    mGame = game;
}

void LevelManager::Shutdown()
{
    UnloadLevel();
    mGame = nullptr;
}

void LevelManager::LoadAndBuildLevel(GameScene scene)
{
    mCurrentScene = scene;

    // Create core actors
    mCollisionQueryActor = new Actor(mGame);
    new ColliderComponent(mCollisionQueryActor, 0, 0, nullptr, CollisionFilter());

    mDebugActor = new DebugActor(mGame);

    mAttackTrailActor = new Actor(mGame);
    new AnimatedParticleSystemComponent(mAttackTrailActor, "AttackTrailAnim", false);

    mWhiteSlashActor = new Actor(mGame);
    new AnimatedParticleSystemComponent(mWhiteSlashActor, "WhiteSlashAnim", false);

    // Load level file
    std::string levelPath = GetLevelPath(scene);
    mLevelData = LoadLevelFromFile(levelPath, mLevelWidth, mLevelHeight);

    if (mLevelData)
    {
        BuildLevel(mLevelData, mLevelWidth, mLevelHeight);
    }
}

void LevelManager::UnloadLevel()
{
    // Mark non-persistent actors for destruction
    for (auto* actor : mActors)
    {
        if (actor->IsPersistent())
            continue;
        actor->SetState(ActorState::Destroy);
    }

    // Delete destroyed actors
    std::vector<Actor*> deadActors;
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

    // Clear pools and lists
    mUpgradeTreatActors.clear();
    mStompActors.clear();
    mFurBallActors.clear();
    mEnemies.clear();
    mWhiteBombActors.clear();
    mWhiteBubbleActors.clear();

    // Reset pointers
    mLevelPortal = nullptr;
    mCurrentBoss = nullptr;

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
}

void LevelManager::ProcessInput(const Uint8* keyState)
{
    for (auto actor : mActors)
    {
        actor->ProcessInput(keyState);
    }
}

void LevelManager::Update(float deltaTime)
{
    UpdateActors(deltaTime);
    UpdateCamera(deltaTime);
    UpdatePortalActivation();

    // Check for level transition
    if (mShadowCat && mLevelPortal && mLevelPortal->IsActive())
    {
        GameScene nextScene;
        if (CheckLevelTransition(mShadowCat->GetPosition(), nextScene))
        {
            // Let Game handle the transition
            mGame->SetScene(nextScene);
        }
    }
}

void LevelManager::UpdateActors(float deltaTime)
{
    mUpdatingActors = true;
    for (auto actor : mActors)
    {
        actor->Update(deltaTime);
    }
    mUpdatingActors = false;

    // Add pending actors
    for (auto pending : mPendingActors)
    {
        mActors.emplace_back(pending);
    }
    mPendingActors.clear();

    // Remove dead actors
    std::vector<Actor*> deadActors;
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

void LevelManager::UpdateCamera(float deltaTime)
{
    if (mShadowCat)
    {
        float targetX = mShadowCat->GetPosition().x;
        float targetY = mShadowCat->GetPosition().y;

        mCameraPos.x = targetX - (GameConstants::WINDOW_WIDTH / 2.0f);
        mCameraPos.y = targetY - (GameConstants::WINDOW_HEIGHT / 2.0f);

        // Lobby doesn't clamp camera
        if (mCurrentScene == GameScene::Lobby)
            return;

        // Clamp camera to level boundaries
        float levelPixelWidth = static_cast<float>(mLevelWidth) * static_cast<float>(GameConstants::TILE_SIZE);
        float levelPixelHeight = static_cast<float>(mLevelHeight) * static_cast<float>(GameConstants::TILE_SIZE);

        float maxCameraX = levelPixelWidth - GameConstants::WINDOW_WIDTH;
        float maxCameraY = levelPixelHeight - GameConstants::WINDOW_HEIGHT;

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

void LevelManager::UpdatePortalActivation()
{
    if (mShadowCat && mLevelPortal)
    {
        int aliveEnemies = CountAliveEnemies();
        int aliveBosses = CountAliveBosses();

        bool shouldActivate = (aliveEnemies == 0 && aliveBosses == 0) || mGame->IsDebugging();

        if (shouldActivate && !mLevelPortal->IsActive())
        {
            SDL_Log("[PORTAL] ACTIVATING - All threats cleared! (Enemies: %d, Bosses: %d)",
                    aliveEnemies, aliveBosses);
            mLevelPortal->Activate();
        }
    }
}

bool LevelManager::CheckLevelTransition(const Vector2& playerPos, GameScene& outNextScene)
{
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
        switch (mCurrentScene)
        {
        case GameScene::Lobby:
            outNextScene = GameScene::Level1;
            SDL_Log("[TRANSITION] Lobby -> Level 1");
            return true;
        case GameScene::Level1:
            outNextScene = GameScene::Level1_Boss;
            return true;
        case GameScene::Level1_Boss:
            outNextScene = GameScene::Level2;
            return true;
        case GameScene::Level2:
            outNextScene = GameScene::Level2_Boss;
            return true;
        case GameScene::Level2_Boss:
            outNextScene = GameScene::Level3;
            return true;
        case GameScene::Level3:
            outNextScene = GameScene::Level3_Boss;
            return true;
        case GameScene::Level3_Boss:
            mGame->SetGameWon(true);
            return false;
        default:
            return false;
        }
    }

    return false;
}

void LevelManager::AddActor(Actor* actor)
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

void LevelManager::RemoveActor(Actor* actor)
{
    auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
    if (iter != mPendingActors.end())
    {
        std::iter_swap(iter, mPendingActors.end() - 1);
        mPendingActors.pop_back();
    }

    iter = std::find(mActors.begin(), mActors.end(), actor);
    if (iter != mActors.end())
    {
        std::iter_swap(iter, mActors.end() - 1);
        mActors.pop_back();
    }
}

void LevelManager::AddPersistentActor(Actor* actor)
{
    mPersistentActors.emplace_back(actor);
}

void LevelManager::RemovePersistentActor(Actor* actor)
{
    auto iter = std::find(mPersistentActors.begin(), mPersistentActors.end(), actor);
    if (iter != mPersistentActors.end())
    {
        std::iter_swap(iter, mPersistentActors.end() - 1);
        mPersistentActors.pop_back();
    }
}

void LevelManager::AddDrawable(DrawComponent* drawable)
{
    mDrawables.emplace_back(drawable);
    std::sort(mDrawables.begin(), mDrawables.end(), [](DrawComponent* a, DrawComponent* b)
              { return a->GetDrawOrder() < b->GetDrawOrder(); });
}

void LevelManager::RemoveDrawable(DrawComponent* drawable)
{
    auto iter = std::find(mDrawables.begin(), mDrawables.end(), drawable);
    if (iter != mDrawables.end())
        mDrawables.erase(iter);
}

void LevelManager::AddCollider(ColliderComponent* collider)
{
    mColliders.emplace_back(collider);
}

void LevelManager::RemoveCollider(ColliderComponent* collider)
{
    auto iter = std::find(mColliders.begin(), mColliders.end(), collider);
    if (iter != mColliders.end())
        mColliders.erase(iter);
}

void LevelManager::RegisterEnemy(EnemyBase* enemy)
{
    if (enemy)
        mEnemies.push_back(enemy);
}

void LevelManager::RegisterBoss(BossBase* boss)
{
    if (boss)
        mCurrentBoss = boss;
}

void LevelManager::UnregisterEnemy(EnemyBase* enemy)
{
    auto iter = std::find(mEnemies.begin(), mEnemies.end(), enemy);
    if (iter != mEnemies.end())
        mEnemies.erase(iter);
}

void LevelManager::UnregisterBoss(BossBase* boss)
{
    if (mCurrentBoss == boss)
        mCurrentBoss = nullptr;
}

int LevelManager::CountAliveEnemies() const
{
    int count = 0;
    for (auto enemy : mEnemies)
    {
        if (enemy && enemy->GetState() == ActorState::Active && !enemy->IsDead())
            count++;
    }
    return count;
}

int LevelManager::CountAliveBosses() const
{
    return (mCurrentBoss && !mCurrentBoss->IsDead()) ? 1 : 0;
}

StompActor* LevelManager::GetStompActor()
{
    for (auto actor : mStompActors)
        if (actor->IsDead())
            return actor;

    StompActor* stomp = new StompActor(mGame);
    mStompActors.push_back(stomp);
    return stomp;
}

FurBallActor* LevelManager::GetFurBallActor()
{
    for (auto actor : mFurBallActors)
        if (actor->IsDead())
            return actor;

    FurBallActor* furball = new FurBallActor(mGame);
    mFurBallActors.push_back(furball);
    return furball;
}

WhiteBombActor* LevelManager::GetWhiteBombActor()
{
    for (auto actor : mWhiteBombActors)
        if (actor->IsDead())
            return actor;

    WhiteBombActor* bomb = new WhiteBombActor(mGame);
    mWhiteBombActors.push_back(bomb);
    return bomb;
}

WhiteBubbleActor* LevelManager::GetWhiteBubbleActor()
{
    for (auto actor : mWhiteBubbleActors)
        if (actor->IsDead())
            return actor;

    WhiteBubbleActor* bubble = new WhiteBubbleActor(mGame);
    mWhiteBubbleActors.push_back(bubble);
    return bubble;
}

UpgradeTreat* LevelManager::GetUpgradeTreatActor()
{
    for (auto actor : mUpgradeTreatActors)
        if (actor->IsCollected())
            return actor;

    UpgradeTreat* treat = new UpgradeTreat(mGame);
    mUpgradeTreatActors.push_back(treat);
    return treat;
}

std::string LevelManager::GetLevelPath(GameScene scene) const
{
    switch (scene)
    {
    case GameScene::Lobby:
        return "../Assets/Levels/Lobby/Lobby.csv";
    case GameScene::Level1:
        return "../Assets/Levels/Level1/Level1.csv";
    case GameScene::Level1_Boss:
        return "../Assets/Levels/Level1_Boss/Level1_Boss.csv";
    case GameScene::Level2:
        return "../Assets/Levels/Level2/Level2.csv";
    case GameScene::Level2_Boss:
        return "../Assets/Levels/Level2_Boss/Level2_Boss.csv";
    case GameScene::Level3:
        return "../Assets/Levels/Level3/Level3.csv";
    case GameScene::Level3_Boss:
        return "../Assets/Levels/Level3_Boss/Level3_Boss.csv";
    default:
        return "../Assets/Levels/Lobby/Lobby.csv";
    }
}

int** LevelManager::LoadLevelFromFile(const std::string& fileName, int& outWidth, int& outHeight)
{
    std::ifstream levelFile(fileName);
    if (!levelFile.is_open())
    {
        SDL_Log("Failed to open level file: %s", fileName.c_str());
        return nullptr;
    }

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

    int** levelData = new int*[outHeight];
    for (int i = 0; i < outHeight; ++i)
    {
        levelData[i] = new int[outWidth];
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

    if (mGame->IsDebugging())
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

void LevelManager::BuildLevel(int** levelData, int width, int height)
{
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            int tileID = levelData[i][j];

            float x = (j * GameConstants::TILE_SIZE) + (GameConstants::TILE_SIZE / 2.0f);
            float y = (i * GameConstants::TILE_SIZE) + (GameConstants::TILE_SIZE / 2.0f);

            Vector2 position(x, y);

            SpawnActorByTileID(tileID, position);
        }
    }

    // Spawn portal at center of last row
    int centerColumn = width / 2;
    int lastRow = height - 1;
    float portalX = (centerColumn * GameConstants::TILE_SIZE) + (GameConstants::TILE_SIZE / 2.0f);
    float portalY = (lastRow * GameConstants::TILE_SIZE) + (GameConstants::TILE_SIZE / 2.0f);

    mLevelPortal = new LevelPortal(mGame);
    mLevelPortal->SetPosition(Vector2(portalX, portalY));

    if (mGame->IsDebugging())
    {
        SDL_Log("[BUILD] Debug mode - showing portal immediately");
        mLevelPortal->Activate();
    }
}

void LevelManager::SpawnActorByTileID(int tileID, const Vector2& position)
{
    if (tileID == 0)
    {
        if (!mShadowCat)
            mShadowCat = new ShadowCat(mGame, position);
        else
            mShadowCat->SetPosition(position);
    }
    else if (tileID == 1)
    {
        new WhiteCat(mGame, position);
    }
    else if (tileID == 2)
    {
        new OrangeCat(mGame, position);
    }
    else if (tileID == 3)
    {
        new SylvesterCat(mGame, position);
    }
    else if (tileID >= 4 && tileID <= 10)
    {
        if (tileID == 9)
        {
            SDL_Log("[BUILD] Skipping tile 9 (carpet) - will be rendered by LevelPortal");
        }
        else
        {
            Block* block = new Block(mGame, tileID);
            block->SetPosition(position);
        }
    }
    else if (tileID == 11)
    {
        new Dummy(mGame, position);
    }
    else if (tileID >= 16 && tileID <= 27)
    {
        Block* block = new Block(mGame, tileID);
        block->SetPosition(position);
    }
    else if (tileID == 12)
    {
        new WhiteBoss(mGame, position);
    }
    else if (tileID == 13)
    {
        new OrangeBoss(mGame, position);
    }
    else if (tileID == 14)
    {
        new SylvesterBoss(mGame, position);
    }
}