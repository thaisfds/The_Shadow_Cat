#include "GameRenderer.h"
#include "Game.h"
#include "LevelManager.h"
#include "SceneManager.h"
#include "GameConstants.h"
#include "Renderer/Renderer.h"
#include "Renderer/Texture.h"
#include "Actors/Characters/Boss.h"
#include "Components/Drawing/DrawComponent.h"

// Singleton instance
GameRenderer& GameRenderer::Instance()
{
    static GameRenderer instance;
    return instance;
}

GameRenderer::GameRenderer()
    : mGame(nullptr)
{
}

GameRenderer::~GameRenderer()
{
    Shutdown();
}

void GameRenderer::Initialize(Game* game)
{
    mGame = game;
}

void GameRenderer::Shutdown()
{
    mGame = nullptr;
}

void GameRenderer::Render()
{
    auto renderer = mGame->GetRenderer();
    renderer->Clear();

    DrawBackground();
    DrawActors();

    // Draw UI
    renderer->DrawAllUI();

    renderer->Present();
}

void GameRenderer::DrawBackground()
{
    auto renderer = mGame->GetRenderer();
    std::string backgroundPath = SceneManager::Instance().GetBackgroundPath();
    Texture* backgroundTexture = renderer->GetTexture(backgroundPath);
    
    if (!backgroundTexture)
        return;

    // Main menu
    if (SceneManager::Instance().GetCurrentScene() == GameScene::MainMenu)
    {
        Vector2 position(GameConstants::WINDOW_WIDTH / 2.0f, GameConstants::WINDOW_HEIGHT / 2.0f);
        Vector2 size(static_cast<float>(backgroundTexture->GetWidth()), static_cast<float>(backgroundTexture->GetHeight()));
        renderer->DrawTexture(position, size, 0.0f, Vector3(1.0f, 1.0f, 1.0f), backgroundTexture, Vector4::UnitRect, Vector2::Zero);
    }
    else
    {
        // Scaled background for levels
        float levelPixelWidth = static_cast<float>(LevelManager::Instance().GetLevelWidth()) * static_cast<float>(GameConstants::TILE_SIZE);
        float levelPixelHeight = static_cast<float>(LevelManager::Instance().GetLevelHeight()) * static_cast<float>(GameConstants::TILE_SIZE);

        float texW = static_cast<float>(backgroundTexture->GetWidth());
        float texH = static_cast<float>(backgroundTexture->GetHeight());

        float scale = std::max(levelPixelWidth / texW, levelPixelHeight / texH);
        float backgroundWidth = texW * scale;
        float backgroundHeight = texH * scale;

        Vector2 position(levelPixelWidth / 2.0f, levelPixelHeight / 2.0f);
        Vector2 size(backgroundWidth, backgroundHeight);

        renderer->DrawTexture(position, size, 0.0f, Vector3(1.0f, 1.0f, 1.0f), backgroundTexture, Vector4::UnitRect, LevelManager::Instance().GetCameraPos());
    }
}

void GameRenderer::DrawActors()
{
    auto renderer = mGame->GetRenderer();
    bool isDebugging = mGame->IsDebugging();

    for (auto drawable : LevelManager::Instance().GetDrawables())
    {
        drawable->Draw(renderer);

        if (isDebugging)
        {
            auto actor = drawable->GetOwner();
            auto boss = dynamic_cast<Boss*>(actor);
            if (boss)
            {
                boss->OnDebugDraw(renderer);
            }

            for (auto comp : actor->GetComponents())
            {
                comp->DebugDraw(renderer);
            }
        }

        for (auto comp : drawable->GetOwner()->GetComponents())
            comp->ComponentDraw(renderer);
    }
}
