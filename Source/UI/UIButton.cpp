#include "UIButton.h"
#include "../Renderer/Texture.h"

UIButton::UIButton(class Game* game, std::function<void()> onClick, const std::string& text, class Font* font,
                   const Vector2 &offset, float scale, float angle, int pointSize, const unsigned wrapLength, int drawOrder)
        :UIText(game, text, font, offset, scale, angle, pointSize, wrapLength, drawOrder)
        ,mOnClick(onClick)
        ,mHighlighted(false)
{
    mNormalColor = mTextColor;
}

UIButton::~UIButton()
{

}


void UIButton::OnClick()
{
    // Call attached handler, if it exists
    if (mOnClick) {
        mOnClick();
    }
}

void UIButton::Draw(class Shader* shader)
{
    if(mHighlighted) {
        SetTextColor(mTextHighlightColor);
    }
    else {
        SetTextColor(mNormalColor);
    }

    UIText::Draw(shader);
}