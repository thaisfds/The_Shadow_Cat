#include "AnimatorComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../Json.h"
#include "../../Renderer/Texture.h"
#include <fstream>
#include <map>

AnimatorComponent::AnimatorComponent(class Actor *owner, const std::string &texPath, const std::string &dataPath,
									 int width, int height, int drawOrder)
	: DrawComponent(owner, drawOrder), mIsPaused(false), mWidth(width), mHeight(height), mTextureFactor(1.0f), mCurrentAnimation(nullptr), mLoopAnimName(""), mRemainingLoops(-1)
	, mAnimSpeed(1.0f), mFrameTimer(0.0f), mCurrentFrameIndex(0)
{
	mSpriteTexture = mOwner->GetGame()->GetRenderer()->GetTexture(texPath);

	if (!dataPath.empty())
	{
		if (!LoadSpriteSheetData(dataPath))
			SDL_Log("Failed to load sprite sheet data for %s", texPath.c_str());
	}
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

	// Create a map to store frames with their indices for proper ordering
	std::map<int, nlohmann::json> orderedFrames;

	for (const auto &[key, value] : spriteSheetData["frames"].items())
	{
		// Extract the frame number from the key (e.g., "MapTileset4.aseprite" -> 4)
		std::string frameName = key;
		size_t dotPos = frameName.find('.');
		if (dotPos != std::string::npos)
		{
			frameName = frameName.substr(0, dotPos);
		}

		// Find the last sequence of digits in the name
		int frameIndex = 0;
		size_t numStart = frameName.find_last_not_of("0123456789");
		if (numStart != std::string::npos && numStart + 1 < frameName.length())
		{
			frameIndex = std::stoi(frameName.substr(numStart + 1));
		}

		orderedFrames[frameIndex] = value;
	}

	// Now add frames in order
	for (const auto &[index, frame] : orderedFrames)
	{
		int x = frame["frame"]["x"].get<int>();
		int y = frame["frame"]["y"].get<int>();
		int w = frame["frame"]["w"].get<int>();
		int h = frame["frame"]["h"].get<int>();
	    
		int duration = frame["duration"].get<int>();

		auto sprite = Sprite{
			Vector4(static_cast<float>(x) / textureWidth, static_cast<float>(y) / textureHeight,
					static_cast<float>(w) / textureWidth, static_cast<float>(h) / textureHeight),
			static_cast<float>(duration) / 1000.0f
		};
		mSpriteSheetData.push_back(sprite);
	}

	return true;
}

void AnimatorComponent::Draw(Renderer *renderer)
{
	if (mIsVisible)
	{
		Vector4 texRect = Vector4::UnitRect;

		if (!mSpriteSheetData.empty())
			texRect = mCurrentAnimation ? mCurrentAnimation->frames[mCurrentFrameIndex]->uv : mSpriteSheetData[0].uv;

		bool flipH = (mOwner->GetScale().x < 0.0f);

		renderer->DrawTexture(
			mOwner->GetPosition(),
			Vector2(mWidth, mHeight),
			mOwner->GetRotation(),
			Vector3(1.0f, 1.0f, 1.0f),
			mSpriteTexture,
			texRect,
			mOwner->GetGame()->GetCameraPos(),
			flipH,
			mTextureFactor);
	}
}

void AnimatorComponent::Update(float deltaTime)
{
    if (mIsPaused || mAnimations.empty()) return;

    mFrameTimer += deltaTime * mAnimSpeed;
    if (mFrameTimer >= mCurrentAnimation->frames[mCurrentFrameIndex]->duration)
    {
        mFrameTimer -= mCurrentAnimation->frames[mCurrentFrameIndex]->duration;
        mCurrentFrameIndex++;
        if (mCurrentFrameIndex >= mCurrentAnimation->frames.size())
        {
            mRemainingLoops--;

            if (mRemainingLoops == 0) SetAnimation(mLoopAnimName); // Return to loop animation after finishing
            else mCurrentFrameIndex = 0; // Loop animation
        }
    }
}

void AnimatorComponent::LoopAnimation(const std::string &name)
{
	if (mLoopAnimName == name) return;

    SetAnimation(name);
	mLoopAnimName = name;
}


void AnimatorComponent::PlayAnimation(const std::string &name, int loops)
{
    SetAnimation(name);
    mRemainingLoops = loops;
}

void AnimatorComponent::SetAnimation(const std::string &name)
{
    auto animIter = mAnimations.find(name);
    if (animIter == mAnimations.end()) return SDL_Log("Animation %s not found!", name.c_str());

    mCurrentAnimation = &animIter->second;
    mCurrentFrameIndex = 0;
    mFrameTimer = 0.0f;
}


void AnimatorComponent::AddAnimation(const std::string &name, const std::vector<int> &spriteNums)
{
	Animation animation;
	
	animation.totalDuration = 0.0f;
	animation.frames.reserve(spriteNums.size());
	for (int frameIndex : spriteNums)
	{
		auto sprite = &mSpriteSheetData[frameIndex];
		animation.frames.push_back(sprite);
		animation.totalDuration += sprite->duration;
	}
	
	mAnimations.emplace(name, animation);
}