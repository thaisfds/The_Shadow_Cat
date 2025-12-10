#pragma once

#include "../Math.h"
#include "../Renderer/Shader.h"
#include "../Game.h"

class UIElement {
public:
    UIElement(class Game* game, const Vector2 &offset, const float scale = 1.0f, const float angle = 0.0f, int drawOrder = 100);
    ~UIElement();

    // Game getter
    class Game* GetGame() { return mGame; }

    // Getters/setters
    const Vector2& GetOffset() const { return mOffset; }
    void SetOffset(const Vector2 &offset) { mOffset = offset; }

    const Vector2& GetAbsolutePos() const { return mAbsolutePos; }
    void SetAbsolutePos(const Vector2 &absPos) { mAbsolutePos = absPos; }

    float GetScale() const { return mScale; }
    void SetScale(const float scale) { mScale = scale; }

    float GetAngle() const { return mAngle; }
    void SetAngle(const float angle) { mAngle = angle; }

    bool IsVisible(const bool isVisible) const { return mIsVisible; }
    void SetIsVisible(const bool isVisible) { mIsVisible = isVisible; }

    int GetDrawOrder() const { return mDrawOrder; }

    virtual void Draw(class Shader* shader) {};

protected:
    class Game* mGame;

    Vector2 mOffset;
    Vector2 mAbsolutePos = Vector2(-1.0f, -1.0f); // ignore if negative
    float mScale;
    float mAngle;

    bool mIsVisible;
    int mDrawOrder;
};
