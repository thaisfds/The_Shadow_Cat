#pragma once

#include <unordered_map>
#include "DrawComponent.h"

struct Sprite
{
    Vector4 uv;
    float duration;
};

struct Animation
{
    std::vector<Sprite*> frames;
    float totalDuration;
};

class AnimatorComponent : public DrawComponent
{
public:
    // (Lower draw order corresponds with further back)
    AnimatorComponent(class Actor *owner, const std::string &animationName,
                      int width, int height, int drawOrder = 100);
    ~AnimatorComponent() override;

    void Draw(Renderer *renderer) override;
    void Update(float deltaTime) override;

    // Use to change the speed of the animation
    void SetAnimSpeed(float speed) { mAnimSpeed = speed; }

    // Set the current active animation
    void LoopAnimation(const std::string &name);
    void PlayAnimation(const std::string &name, int loops);
    void PlayAnimationOnce(const std::string &name) { PlayAnimation(name, 1); }

    void ResetAnimation();

    // Use to pause/unpause the animation
    void SetIsPaused(bool pause) { mIsPaused = pause; }

    // Add an animation of the corresponding name to the animation map
    void AddAnimation(const std::string &name, const std::vector<int> &images);

    float GetAnimationDuration(const std::string &name);
    float GetCurrentAnimationDuration() const;

private:
    void SetAnimation(const std::string &name);

    bool LoadAnimationData(const std::string &animationName);
    bool LoadSpriteSheetData(const std::string &dataPath);

    // Sprite sheet texture
    class Texture *mSpriteTexture;

    // Vector of sprites
    std::vector<Sprite> mSpriteSheetData;

    // Map of animation name to vector of textures corresponding to the animation
    std::unordered_map<std::string, Animation> mAnimations;

    // Name of current animation
    std::string mLoopAnimName;

    Animation *mCurrentAnimation;

    float mFrameTimer = 0.0f;
    int mCurrentFrameIndex = 0;
    int mRemainingLoops;

    // How fast should the animation run
    float mAnimSpeed = 1.0f;

    // Whether or not the animation is paused (defaults to false)
    bool mIsPaused = false;

    // Size
    int mWidth;
    int mHeight;

    float mTextureFactor;
};
