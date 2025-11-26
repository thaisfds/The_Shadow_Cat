#include "UIImage.h"
#include "../Renderer/Texture.h"
#include "../Renderer/Shader.h"
#include "../Renderer/Renderer.h"
#include "../Actors/Actor.h"
#include "../Game.h"
#include "../GameConstants.h"


UIImage::UIImage(class Game* game, const Vector2 &offset, const float scale, const float angle, int drawOrder)
        :UIElement(game, offset, scale, angle, drawOrder)
        ,mTexture(nullptr)
{

}

UIImage::UIImage(class Game* game, const std::string &imagePath, const Vector2 &offset, const float scale, const float angle, int drawOrder)
        :UIElement(game, offset, scale, angle)
{
    mTexture = GetGame()->GetRenderer()->GetTexture(imagePath);
}

UIImage::~UIImage()
{

}

void UIImage::Draw(class Shader* shader)
{
    if(!mTexture || !mIsVisible)
        return;

    // Scale the quad by the width/height of texture
    Matrix4 scaleMat = Matrix4::CreateScale(static_cast<float>(mTexture->GetWidth()) * mScale,
                                            static_cast<float>(mTexture->GetHeight()) * mScale, 1.0f);

    Matrix4 rotMat = Matrix4::CreateRotationZ(mAngle);

    // Translate to position on screen
    Matrix4 transMat = Matrix4::CreateTranslation(Vector3(mOffset.x + GameConstants::WINDOW_WIDTH / 2, mOffset.y + GameConstants::WINDOW_HEIGHT / 2, 0.0f));

    // Set world transform
    Matrix4 world = scaleMat * rotMat * transMat;
    shader->SetMatrixUniform("uWorldTransform", world);

    shader->SetVectorUniform("uColor", Color::White);
    shader->SetVectorUniform("uTexRect", Vector4::UnitRect);
    shader->SetVectorUniform("uCameraPos", Vector2::Zero);

    // Set uTextureFactor
    shader->SetFloatUniform("uTextureFactor", 1.0f);

    // Set current texture
    mTexture->SetActive();

    // Draw quad
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}