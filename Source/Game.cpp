#include <algorithm>
#include <vector>
#include <map>
#include <fstream>
#include "CSV.h"
#include "Game.h"
#include "Components/Drawing/DrawComponent.h"
#include "Components/Physics/RigidBodyComponent.h"
#include "Random.h"
#include "Actors/Actor.h"
#include "Actors/Block.h"
#include "Actors/Goomba.h"
#include "Actors/Spawner.h"
#include "Actors/ShadowCat.h"

Game::Game()
    : mWindow(nullptr), mRenderer(nullptr), mTicksCount(0), mIsRunning(true), mIsDebugging(false), mUpdatingActors(false), mCameraPos(Vector2::Zero), mShadowCat(nullptr), mLevelData(nullptr)
{
}

bool Game::Initialize()
{
    Random::Init();

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow("The Shadow Cat", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = new Renderer(mWindow);
    mRenderer->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Init all game actors
    InitializeActors();

    mTicksCount = SDL_GetTicks();

    return true;
}

void Game::InitializeActors()
{

    mLevelData = LoadLevel("../Assets/Levels/Level1-1/level1-1.csv", LEVEL_WIDTH, LEVEL_HEIGHT);

    if (mLevelData)
    {
        BuildLevel(mLevelData, LEVEL_WIDTH, LEVEL_HEIGHT);
    }
}

int **Game::LoadLevel(const std::string &fileName, int width, int height)
{
    int **levelData = new int *[height];
    for (int i = 0; i < height; ++i)
    {
        levelData[i] = new int[width];
    }

    std::ifstream levelFile(fileName);
    if (!levelFile.is_open())
    {
        SDL_Log("Failed to open level file: %s", fileName.c_str());
        for (int i = 0; i < height; ++i)
        {
            delete[] levelData[i];
        }
        delete[] levelData;
        return nullptr;
    }

    std::string line;

    for (int i = 0; i < height; ++i)
    {
        if (std::getline(levelFile, line))
        {
            std::vector<int> row = CSVHelper::Split(line);
            for (int j = 0; j < width; ++j)
            {
                if (j < row.size())
                {
                    levelData[i][j] = row[j];
                }
                else
                {
                    levelData[i][j] = 0;
                }
            }
        }
    }

    SDL_Log("--- Conteúdo do Nível CSV ---");
    for (int i = 0; i < height; ++i)
    {
        std::string rowStr = "";
        for (int j = 0; j < width; ++j)
        {
            rowStr += std::to_string(levelData[i][j]) + " ";
        }
        SDL_Log("%s", rowStr.c_str());
    }
    SDL_Log("-----------------------------");

    return levelData;
}

void Game::BuildLevel(int **levelData, int width, int height)
{
    const std::string BLOCK_TEXTURE = "../Assets/Sprites/Blocks/BlockA.png";

    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            int tileID = levelData[i][j];

            float x = (j * TILE_SIZE) + (TILE_SIZE / 2.0f);
            float y = (i * TILE_SIZE) + (TILE_SIZE / 2.0f);

            Vector2 position(x, y);

            switch (tileID)
            {
            case 0:
            {
                auto block = new Block(this, "../Assets/Sprites/Blocks/BlockA.png");
                block->SetPosition(position);
                break;
            }

            case 1:
            {
                auto block = new Block(this, "../Assets/Sprites/Blocks/BlockC.png");
                block->SetPosition(position);
                break;
            }

            case 2:
            {
                auto block = new Block(this, "../Assets/Sprites/Blocks/BlockF.png");
                block->SetPosition(position);
                break;
            }

            case 4:
            {
                auto block = new Block(this, "../Assets/Sprites/Blocks/BlockB.png");
                block->SetPosition(position);
                break;
            }

            case 5:
            {
                auto block = new Block(this, "../Assets/Sprites/Blocks/BlockE.png");
                block->SetPosition(position);
                break;
            }

            case 6:
            {
                auto block = new Block(this, "../Assets/Sprites/Blocks/BlockI.png");
                block->SetPosition(position);
                break;
            }

            case 8:
            {
                auto block = new Block(this, "../Assets/Sprites/Blocks/BlockD.png");
                block->SetPosition(position);
                break;
            }

            case 9:
            {
                auto block = new Block(this, "../Assets/Sprites/Blocks/BlockH.png");
                block->SetPosition(position);
                break;
            }

            case 12:
            {
                auto block = new Block(this, "../Assets/Sprites/Blocks/BlockG.png");
                block->SetPosition(position);
                break;
            }

            case 16:
            {
                mShadowCat = new ShadowCat(this);
                mShadowCat->SetPosition(position);
                break;
            }

            default:
                break;
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
        int sleepTime = (1000 / FPS) - (SDL_GetTicks() - mTicksCount);
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
        mCameraPos.x = targetX - (WINDOW_WIDTH / 2.0f);
        mCameraPos.y = targetY - (WINDOW_HEIGHT / 2.0f);
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

    for (auto drawable : mDrawables)
    {
        drawable->Draw(mRenderer);

        if (mIsDebugging)
        {
            // Call draw for actor components
            for (auto comp : drawable->GetOwner()->GetComponents())
            {
                comp->DebugDraw(mRenderer);
            }
        }
    }

    // Swap front buffer and back buffer
    mRenderer->Present();
}

void Game::Shutdown()
{
    while (!mActors.empty())
    {
        delete mActors.back();
    }

    // Delete level data
    if (mLevelData)
    {
        for (int i = 0; i < LEVEL_HEIGHT; ++i)
        {
            delete[] mLevelData[i];
        }
        delete[] mLevelData;
        mLevelData = nullptr;
    }

    mRenderer->Shutdown();
    delete mRenderer;
    mRenderer = nullptr;

    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}