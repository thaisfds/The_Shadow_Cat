#include "Texture.h"

Texture::Texture()
    : mTextureID(0)
      , mWidth(0)
      , mHeight(0) {
}

Texture::~Texture() {
}

bool Texture::Load(const std::string &filePath) {

    SDL_Surface *surf = IMG_Load(filePath.c_str());
    if (!surf) {
        SDL_Log("Failed to load texture file %s", filePath.c_str());
        return false;
    }

    mWidth = surf->w;
    mHeight = surf->h;

    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    GLenum format = GL_RGB;

    if (surf->format->BytesPerPixel == 4) {
        format = GL_RGBA;
    } else if (surf->format->BytesPerPixel == 3) {
        format = GL_RGB;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, mWidth, mHeight, 0, format,GL_UNSIGNED_BYTE, surf->pixels);

    SDL_FreeSurface(surf);

    return true;
}

void Texture::CreateFromSurface(SDL_Surface* surface)
{
    mWidth = surface->w;
    mHeight = surface->h;

    // Generate a GL texture
    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, surface->pitch / 4);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 surface->pixels);

    // Use linear filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::Unload() {
    glDeleteTextures(1, &mTextureID);
    mTextureID = 0;
    mWidth = 0;
    mHeight = 0;
}

void Texture::SetActive(int index) const {
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
}
