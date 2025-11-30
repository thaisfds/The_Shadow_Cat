#include "AnimatorComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../Json.h"
#include "../../Renderer/Texture.h"
#include <fstream>
#include <map>

const std::string ANIMATION_DATA_PATH = "../Assets/AnimationData/";

AnimatorComponent::AnimatorComponent(class Actor *owner, const std::string &animationName,
									 int width, int height, int drawOrder)
	: DrawComponent(owner, drawOrder), mIsPaused(false), mWidth(width), mHeight(height), mTextureFactor(1.0f), mCurrentAnimation(nullptr), mLoopAnimName(""), mRemainingLoops(-1)
	, mAnimSpeed(1.0f), mFrameTimer(0.0f), mCurrentFrameIndex(0)
{
	bool loaded = LoadAnimationData(animationName);
	if (!loaded)
		mSpriteTexture = mOwner->GetGame()->GetRenderer()->GetTexture("../Assets/Sprites/NoTexture/NoTexture.png");
}

bool AnimatorComponent::LoadAnimationData(const std::string &animationName)
{
	std::ifstream animFile(ANIMATION_DATA_PATH + animationName + ".json");

	if (!animFile.is_open())
	{
		SDL_Log("Failed to open animation file: %s", (animationName).c_str());
		return false;
	}

	nlohmann::json animData = nlohmann::json::parse(animFile);
	if (animData.is_null())
	{
		SDL_Log("Failed to parse animation file: %s", (animationName).c_str());
		return false;
	}

	std::string texturePath = animData["image"].get<std::string>();
	if (texturePath.empty())
	{
		SDL_Log("No texture path specified in animation file: %s", (animationName).c_str());
		return false;
	}

	mSpriteTexture = mOwner->GetGame()->GetRenderer()->GetTexture(texturePath);

	std::string spriteSheetDataPath = animData["spriteSheetData"].get<std::string>();
	if (spriteSheetDataPath.empty())
	{
		SDL_Log("No sprite sheet data path specified in animation file: %s", (animationName).c_str());
		return false;
	}

	if (!LoadSpriteSheetData(spriteSheetDataPath))
	{
		SDL_Log("Failed to load sprite sheet data for %s", texturePath.c_str());
		return false;
	}

	for (const auto& [animName, frameIndices] : animData["animations"].items())
	{
		std::vector<int> indices;
		if (frameIndices.is_array())
			indices = frameIndices.get<std::vector<int>>();
		else if (frameIndices.is_object() && frameIndices.contains("start") && frameIndices.contains("end")) 
		{
			int start = frameIndices["start"].get<int>();
			int end = frameIndices["end"].get<int>();
			for (int i = start; i <= end; ++i) indices.push_back(i);
		}
		AddAnimation(animName, indices);
	}
	if (mAnimations.empty())
		for (auto spriteIndex = 0; spriteIndex < mSpriteSheetData.size(); ++spriteIndex)
			AddAnimation(std::to_string(spriteIndex), { spriteIndex });

	std::string baseAnim = animData.contains("base") ? animData["base"].get<std::string>() : mAnimations.begin()->first;
	LoopAnimation(baseAnim);
	return true;
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
	if (mIsVisible && mSpriteTexture)
	{
		Vector4 texRect = Vector4::UnitRect;

		if (!mSpriteSheetData.empty())
			texRect = mCurrentAnimation ? mCurrentAnimation->frames[mCurrentFrameIndex]->uv : mSpriteSheetData[0].uv;

		bool flipH = (mOwner->GetScale().x < 0.0f);
		bool flipV = (mOwner->GetScale().y < 0.0f);

		renderer->DrawTexture(
			mOwner->GetPosition(),
			Vector2(mWidth, mHeight),
			mOwner->GetRotation(),
			Vector3(1.0f, 1.0f, 1.0f),
			mSpriteTexture,
			texRect,
			mOwner->GetGame()->GetCameraPos(),
			flipH,
			flipV,
			mTextureFactor);
	}
}

void AnimatorComponent::Update(float deltaTime)
{
	if (mIsPaused || mCurrentAnimation == nullptr) return;

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

	SetAnimation(name, false);
	mLoopAnimName = name;
}


void AnimatorComponent::PlayAnimation(const std::string &name, int loops, bool reset)
{
	SetAnimation(name, reset);
	mRemainingLoops = loops;
}

void AnimatorComponent::ResetAnimation()
{
	mCurrentFrameIndex = 0;
	mFrameTimer = 0.0f;
}

void AnimatorComponent::SetAnimation(const std::string &name, bool reset)
{
	auto animIter = mAnimations.find(name);
	if (animIter == mAnimations.end()) return SDL_Log("Animation %s not found!", name.c_str());
	if (!reset && mCurrentAnimation == &animIter->second) return;

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

float AnimatorComponent::GetAnimationDuration(const std::string &name)
{
	auto iter = mAnimations.find(name);
	if (iter == mAnimations.end()) return 0.0f;
	return iter->second.totalDuration;
}

float AnimatorComponent::GetCurrentAnimationDuration() const
{
	if (!mCurrentAnimation) return 0.0f;
	return mCurrentAnimation->totalDuration;
}