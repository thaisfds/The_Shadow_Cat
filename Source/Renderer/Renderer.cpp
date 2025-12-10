#include <GL/glew.h>
#include "Renderer.h"
#include "Shader.h"
#include "VertexArray.h"
#include "Texture.h"
#include "../Game.h"
#include "../GameConstants.h"
#include "../UI/UIElement.h"

Renderer::Renderer(SDL_Window *window)
    : mBaseShader(nullptr),
    mWindow(window),
    mContext(nullptr),
    mOrthoProjection(Matrix4::Identity),
    mScreenWidth(854.0f),
    mScreenHeight(480.0f)
{
}

Renderer::~Renderer()
{
    delete mSpriteVerts;
    mSpriteVerts = nullptr;
}

bool Renderer::Initialize(float width, float height)
{
    mScreenWidth = width;
    mScreenHeight = height;

    // Specify version 3.3 (core profile)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Enable double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // Force OpenGL to use hardware acceleration
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    // Turn on vsync
    SDL_GL_SetSwapInterval(1);

    // Create an OpenGL context
    mContext = SDL_GL_CreateContext(mWindow);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        SDL_Log("Failed to initialize GLEW.");
        return false;
    }

    // Make sure we can create/compile shaders
    if (!LoadShaders())
    {
        SDL_Log("Failed to load shaders.");
        return false;
    }

    // Create quad for drawing sprites
    CreateSpriteVerts();

    // Set the clear color to light grey
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Enable alpha blending on textures
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Create orthografic projection matrix
    mOrthoProjection = Matrix4::CreateOrtho(0.0f, width, height, 0.0f, -1.0f, 1.0f);
    mBaseShader->SetMatrixUniform("uOrthoProj", mOrthoProjection);

    mBaseShader->SetIntegerUniform("uTexture", 0);
    // Activate shader
    mBaseShader->SetActive();

    return true;
}

void Renderer::AddUIElement(UIElement *comp)
{
    mUIComps.emplace_back(comp);

    std::stable_sort(mUIComps.begin(), mUIComps.end(),[](UIElement* a, UIElement* b) {
        return a->GetDrawOrder() < b->GetDrawOrder();
    });
}

void Renderer::RemoveUIElement(UIElement *comp)
{
    auto iter = std::find(mUIComps.begin(), mUIComps.end(), comp);
    mUIComps.erase(iter);
}

void Renderer::Shutdown()
{
    // Destroy textures
    for (auto i : mTextures)
    {
        i.second->Unload();
        delete i.second;
    }
    mTextures.clear();

    // Destroy fonts
    for (auto i : mFonts)
    {
        i.second->Unload();
        delete i.second;
    }
    mFonts.clear();

    mBaseShader->Unload();
    delete mBaseShader;

    SDL_GL_DeleteContext(mContext);
    SDL_DestroyWindow(mWindow);
}

void Renderer::Clear()
{
    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::Draw(RendererMode mode, const Matrix4 &modelMatrix, const Vector2 &cameraPos, VertexArray *vertices,
                    const Vector3 &color, Texture *texture, const Vector4 &textureRect, float textureFactor)
{
    mBaseShader->SetMatrixUniform("uWorldTransform", modelMatrix);
    mBaseShader->SetVectorUniform("uColor", color);
    mBaseShader->SetVectorUniform("uTexRect", textureRect);
    mBaseShader->SetVectorUniform("uCameraPos", cameraPos);

    if (vertices)
    {
        vertices->SetActive();
    }

    if (texture)
    {
        texture->SetActive();
        mBaseShader->SetFloatUniform("uTextureFactor", textureFactor);
    }
    else
    {
        mBaseShader->SetFloatUniform("uTextureFactor", 0.0f);
    }

    if (mode == RendererMode::LINES)
    {
        glDrawElements(GL_LINE_LOOP, vertices->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
    }
    else if (mode == RendererMode::TRIANGLES)
    {
        glDrawElements(GL_TRIANGLES, vertices->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
    }
}

void Renderer::DrawRect(const Vector2 &position, const Vector2 &size, float rotation, const Vector3 &color,
                        const Vector2 &cameraPos, RendererMode mode)
{
    Matrix4 model = Matrix4::CreateScale(Vector3(size.x, size.y, 1.0f)) *
                    Matrix4::CreateRotationZ(rotation) *
                    Matrix4::CreateTranslation(Vector3(position.x, position.y, 0.0f));

    Draw(mode, model, cameraPos, mSpriteVerts, color);
}

void Renderer::DrawTexture(const Vector2 &position, const Vector2 &size, float rotation, const Vector3 &color,
                           Texture *texture, const Vector4 &textureRect, const Vector2 &cameraPos, bool flipH,
                           bool flipV, float textureFactor)
{
    float flipFactorH = flipH ? -1.0f : 1.0f;
    float flipFactorV = flipV ? -1.0f : 1.0f;

    Matrix4 model = Matrix4::CreateScale(Vector3(size.x * flipFactorH, size.y * flipFactorV, 1.0f)) *
                    Matrix4::CreateRotationZ(rotation) *
                    Matrix4::CreateTranslation(Vector3(position.x, position.y, 0.0f));

    Draw(RendererMode::TRIANGLES, model, cameraPos, mSpriteVerts, color, texture, textureRect, textureFactor);
}

void Renderer::DrawGeometry(const Vector2 &position, const Vector2 &size, float rotation, const Vector3 &color,
                            const Vector2 &cameraPos, VertexArray *vertexArray, RendererMode mode)
{
    Matrix4 model = Matrix4::CreateScale(Vector3(size.x, size.y, 1.0f)) *
                    Matrix4::CreateRotationZ(rotation) *
                    Matrix4::CreateTranslation(Vector3(position.x, position.y, 0.0f));

    Draw(mode, model, cameraPos, vertexArray, color);
}

void Renderer::DrawCircle(const Vector2 &center, float radius, const Vector3 &color,
                            const Vector2 &cameraPos)
{
    int numVerts = 64;
	float internalAngle = Math::ToRadians(360 / (float)numVerts);
	float verts[numVerts * 4]; // 4 floats per vertex: x, y, u, v
	unsigned int indices[numVerts];
	for (int i = 0; i < numVerts; i++)
	{
		verts[i * 4 + 0] = radius * cos(internalAngle * i);  // x
		verts[i * 4 + 1] = radius * sin(internalAngle * i);  // y
		verts[i * 4 + 2] = 0.0f;  // u (unused)
		verts[i * 4 + 3] = 0.0f;  // v (unused)
	}
	for (int i = 0; i < numVerts; i++) indices[i] = i;

	VertexArray circleVerts(verts, numVerts, indices, numVerts);
    Matrix4 model = Matrix4::CreateTranslation(Vector3(center.x, center.y, 0.0f));
    
    Draw(RendererMode::LINES, model, cameraPos, &circleVerts, color);
}

void Renderer::DrawPolygon(const std::vector<Vector2> &points, const Vector3 &color, const Vector2 &offset,
                           const Vector2 &cameraPos)
{
    size_t numVerts = points.size();
    if (numVerts < 2) return;

    float verts[numVerts * 4]; // 4 floats per vertex: x, y, u, v
    unsigned int indices[numVerts];

    for (size_t i = 0; i < numVerts; i++)
    {
        verts[i * 4 + 0] = points[i].x + offset.x;  // x
        verts[i * 4 + 1] = points[i].y + offset.y;  // y
        verts[i * 4 + 2] = 0.0f;                     // u (unused)
        verts[i * 4 + 3] = 0.0f;                     // v (unused)
        indices[i] = static_cast<unsigned int>(i);
    }

    VertexArray polygonVerts(verts, static_cast<unsigned int>(numVerts), indices, static_cast<unsigned int>(numVerts));
    Matrix4 model = Matrix4::Identity;

    Draw(RendererMode::LINES, model, cameraPos, &polygonVerts, color);
}

void Renderer::DrawAllUI() {
    mSpriteVerts->SetActive();

    for (auto ui : mUIComps) {
        ui->Draw(mBaseShader);
    }
}

void Renderer::Present()
{
    // Swap the buffers
    SDL_GL_SwapWindow(mWindow);
}

bool Renderer::LoadShaders()
{   
    // Create base shader
    mBaseShader = new Shader();
    if (!mBaseShader->Load("../Shaders/Base"))
    {
        return false;
    }

    mBaseShader->SetActive();

    return true;
}

void Renderer::CreateSpriteVerts()
{
    float verts[] = {
        -0.5f, 0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f};

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0};

    mSpriteVerts = new VertexArray(verts, 4, indices, 6);
}

Texture *Renderer::GetTexture(const std::string &fileName)
{
    Texture *tex = nullptr;
    auto iter = mTextures.find(fileName);
    if (iter != mTextures.end())
    {
        tex = iter->second;
    }
    else
    {
        tex = new Texture();
        if (tex->Load(fileName))
        {
            mTextures.emplace(fileName, tex);
            return tex;
        }
        else
        {
            delete tex;
            return nullptr;
        }
    }
    return tex;
}

Font* Renderer::GetFont(const std::string& fileName)
{
    auto iter = mFonts.find(fileName);
    if (iter != mFonts.end())
    {
        return iter->second;
    }
    else
    {
        Font* font = new Font();
        if (font->Load(fileName))
        {
            mFonts.emplace(fileName, font);
        }
        else
        {
            font->Unload();
            delete font;
            font = nullptr;
        }
        return font;
    }
}

void Renderer::UpdateViewport(int windowWidth, int windowHeight)
{
    mScreenWidth = windowWidth;
    mScreenHeight = windowHeight;

    float targetAspect = GameConstants::WINDOW_WIDTH / (float)GameConstants::WINDOW_HEIGHT;
    float windowAspect = (float)windowWidth / (float)windowHeight;
    
    int viewportX = 0;
    int viewportY = 0;
    int viewportWidth = windowWidth;
    int viewportHeight = windowHeight;
    
    if (windowAspect > targetAspect)
    {
        // Window is wider - add black bars on sides
        viewportWidth = (int)(windowHeight * targetAspect);
        viewportX = (windowWidth - viewportWidth) / 2;
    }
    else
    {
        // Window is taller - add black bars on top/bottom
        viewportHeight = (int)(windowWidth / targetAspect);
        viewportY = (windowHeight - viewportHeight) / 2;
    }
    
    glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
}