#include "texture.h"
#include "syslog.h"
#include <memory>
#include <SDL2/SDL_image.h>

Texture::Texture() : mTex(0), mWidth(0), mHeight(0)
{
    glGenTextures(1, &this->mTex);
}

Texture::Texture(char const * fileName) : Texture()
{
    this->load(fileName);
}

Texture::~Texture()
{
    glDeleteTextures(1, &this->mTex);
    this->mTex = 0;
}

void Texture::init(GLenum format, uint width, uint height)
{
    GLint last_texture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);

    glBindTexture(GL_TEXTURE_2D, this->mTex);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        format,
        width, height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, last_texture);

    this->mWidth = width;
    this->mHeight = height;
}

void Texture::load(char const * fileName)
{
    assert(fileName != nullptr);
    std::unique_ptr<SDL_Surface, std::function<void (SDL_Surface*)>> surface(IMG_Load(fileName), SDL_FreeSurface);
    if(surface == nullptr) {
        syslog::error("Failed to create texture: ", IMG_GetError());
        return;
    }
    GLint last_texture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);

    GLenum format = GL_RGB;
    if(surface->format->BytesPerPixel == 4)
    {
        if(surface->format->Rmask == 0xFF) {
            format = GL_RGBA;
        } else {
            format = GL_BGRA;
        }
    }
    else
    {
        if(surface->format->Rmask == 0xFF) {
            format = GL_RGB;
        } else {
            format = GL_BGR;
        }
    }

    glBindTexture(GL_TEXTURE_2D, this->mTex);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        surface->w, surface->h,
        0,
        format,
        GL_UNSIGNED_BYTE,
        surface->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, last_texture);

    this->mWidth = surface->w;
    this->mHeight = surface->h;
}
