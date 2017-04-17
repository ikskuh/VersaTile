#ifndef TEXTURE_H
#define TEXTURE_H

#include "imgui/imgui.h"
#include <GL/gl3w.h>
#include <SDL2/SDL.h>

class Texture
{
private:
    GLuint mTex;
    uint mWidth, mHeight;

public:
    Texture();
    Texture(char const * fileName);

    Texture(Texture &&other) = delete;
    Texture(const Texture &other) = delete;

    ~Texture();

    void init(GLenum format, uint width, uint height);

    void load(char const * fileName);

    GLuint id() const { return this->mTex; }

    uint width() const { return this->mWidth; }

    uint height() const { return this->mHeight; }

    ImVec2 size() const { return ImVec2(this->mWidth, this->mHeight); }

    operator ImTextureID() const {
        return reinterpret_cast<ImTextureID>(this->mTex);
    }
};

#endif // TEXTURE_H
