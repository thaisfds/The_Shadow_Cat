#include "AnimatorComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../Json.h"
#include "../../Renderer/Texture.h"
#include <fstream>

AnimatorComponent::AnimatorComponent(class Actor *owner, const std::string &texPath, const std::string &dataPath,
                                     int width, int height, int drawOrder)
    : DrawComponent(owner, drawOrder), mAnimTimer(0.0f), mIsPaused(false), mWidth(width), mHeight(height), mTextureFactor(1.0f)
{
    mSpriteTexture = mOwner->GetGame()->GetRenderer()->GetTexture(texPath);
}

AnimatorComponent::~AnimatorComponent()
{
    mAnimations.clear();
    mSpriteSheetData.clear();
}

bool AnimatorComponent::LoadSpriteSheetData(const std::string &dataPath)
{
    // Load sprite sheet data and return false if it fails
    std::ifstream spriteSheetFile(dataPath);

    if (!spriteSheetFile.is_open())
    {
        SDL_Log("Failed to open sprite sheet data file: %s", dataPath.c_str());
        return false;
    }

    nlohmann::json spriteSheetData = nlohmann::json::parse(spriteSheetFile);

    if (spriteSheetData.is_null())
    {
        SDL_Log("Failed to parse sprite sheet data file: %s", dataPath.c_str());
        return false;
    }

    auto textureWidth = static_cast<float>(spriteSheetData["meta"]["size"]["w"].get<int>());
    auto textureHeight = static_cast<float>(spriteSheetData["meta"]["size"]["h"].get<int>());

    for (const auto &frame : spriteSheetData["frames"])
    {

        int x = frame["frame"]["x"].get<int>();
        int y = frame["frame"]["y"].get<int>();
        int w = frame["frame"]["w"].get<int>();
        int h = frame["frame"]["h"].get<int>();

        mSpriteSheetData.emplace_back(static_cast<float>(x) / textureWidth, static_cast<float>(y) / textureHeight,
                                      static_cast<float>(w) / textureWidth, static_cast<float>(h) / textureHeight);
    }

    return true;
}

void AnimatorComponent::Draw(Renderer *renderer)
{
    if (mIsVisible)
    {
        Vector4 texRect = Vector4::UnitRect;

        if (!mSpriteSheetData.empty())
        {
            texRect = mSpriteSheetData[0];
        }

        renderer->DrawTexture(
            mOwner->GetPosition(),
            Vector2(mWidth, mHeight),
            mOwner->GetRotation(),
            Vector3(1.0f, 1.0f, 1.0f),
            mSpriteTexture,
            texRect,
            mOwner->GetGame()->GetCameraPos(),
            false,
            mTextureFactor);
    }
}

void AnimatorComponent::Update(float deltaTime)
{
}

void AnimatorComponent::SetAnimation(const std::string &name)
{
}

void AnimatorComponent::AddAnimation(const std::string &name, const std::vector<int> &spriteNums)
{
}