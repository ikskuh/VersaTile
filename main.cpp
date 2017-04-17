// ImGui - standalone example application for SDL2 + OpenGL
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.

#include <imgui.h>
#include "imgui_impl_sdl_gl3.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <GL/gl3w.h>    // This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <memory>
#include <nfd.h>

static void sysOpenGLDebug(GLenum,GLenum,GLuint,GLenum,GLsizei,const GLchar*,const void *);

class Texture
{
private:
    GLuint mTex;
    uint mWidth, mHeight;

public:
    Texture() : mTex(0), mWidth(0), mHeight(0)
    {
        glGenTextures(1, &this->mTex);
    }

    Texture(char const * fileName) : Texture()
    {
        this->load(fileName);
    }

    Texture(Texture &&other) = delete;

    Texture(const Texture &other) = delete;

    ~Texture()
    {
        glDeleteTextures(1, &this->mTex);
        this->mTex = 0;
    }

    void load(char const * fileName)
    {
        if(fileName == nullptr) {
            throw "Missing filename!";
        }
        std::unique_ptr<SDL_Surface, std::function<void (SDL_Surface*)>> surface(IMG_Load(fileName), SDL_FreeSurface);
        if(surface == nullptr) {
            throw IMG_GetError();
        }
        GLint last_texture;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);

        std::cout << surface->w << ", " << surface->h << ", " << surface->pixels << std::endl;
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

    GLuint id() const { return this->mTex; }

    uint width() const { return this->mWidth; }

    uint height() const { return this->mHeight; }

    ImVec2 size() const { return ImVec2(this->mWidth, this->mHeight); }

    operator ImTextureID() const {
        return reinterpret_cast<ImTextureID>(this->mTex);
    }
};

namespace ImGui
{
    static void TooltipText(char const * fmt, ...)
    {
        if(ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            va_list args;
            va_start(args, fmt);
            ImGui::TextV(fmt, args);
            va_end(args);
            ImGui::EndTooltip();
        }
    }

    static bool ToolbarButton(Texture const & image, char const * toolTip)
    {
        auto result = ImGui::ImageButton(image, image.size());
        ImGui::TooltipText(toolTip);
        ImGui::SameLine();
        return result;
    }
}

int main(int, char**)
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }
    if(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == 0)
    {
        std::cerr
            << "[ERR] Could not initialize SDL2_image: "
            << IMG_GetError()
            << std::endl;
        return -1;
    }

    // Setup window
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    SDL_Window *window = SDL_CreateWindow("Versa-Tile Editor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
    gl3wInit();

    glDebugMessageCallback(&sysOpenGLDebug, nullptr);
    glEnable(GL_DEBUG_OUTPUT);

    // Setup ImGui binding
    ImGui_ImplSdlGL3_Init(window);

    Texture imgOpen("data/open.png");
    Texture imgSave("data/save.png");
    Texture imgClose("data/close.png");

    // Main loop
    bool done = false;
    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if(ImGui_ImplSdlGL3_ProcessEvent(&event))
                continue;
            switch(event.type)
            {
                case SDL_QUIT:
                    done = true;
                    break;
            }
        }
        ImGui_ImplSdlGL3_NewFrame(window);

        if(ImGui::BeginMainMenuBar())
        {
            if(ImGui::BeginMenu("File"))
            {
                ImGui::MenuItem("Open");
                ImGui::Separator();
                if(ImGui::MenuItem("Quit"))
                {
                    done = true;
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Examples"))
            {
                ImGui::MenuItem("Main menu bar");
                ImGui::MenuItem("Console");
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        float w = ImGui::GetIO().DisplaySize.x;
        ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f), ImGuiSetCond_Always);
        ImGui::SetNextWindowSizeConstraints(
            ImVec2(w, 0),
            ImVec2(w, 100));
        auto style = 0
                | ImGuiWindowFlags_NoCollapse
                | ImGuiWindowFlags_NoResize
                | ImGuiWindowFlags_NoBringToFrontOnFocus
                | ImGuiWindowFlags_NoTitleBar
                ;
        if(ImGui::Begin("ToolBar", nullptr, style))
        {
            // Toolbar inserted here
            if(ImGui::ToolbarButton(imgOpen, "Open file"))
            {
                nfdchar_t *path;
                auto result = NFD_OpenDialog(nullptr, nullptr, &path);
                switch(result)
                {
                    case NFD_OKAY:
                        std::cout << "Open file: " << path << std::endl;
                        break;
                    case NFD_CANCEL:
                        break;
                    case NFD_ERROR:
                        std::cerr << "[ERR] " << NFD_GetError() << std::endl;
                        break;
                }

            }
            if(ImGui::ToolbarButton(imgSave, "Save file"))
            {

            }
            if(ImGui::ToolbarButton(imgClose, "Close file"))
            {

            }
            ImGui::End();
        }

        // Rendering
        glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render everything else here...

        ImGui::Render();

        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    ImGui_ImplSdlGL3_Shutdown();
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

static void sysOpenGLDebug(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar * message,
    const void *userParam)
{
    std::ostream &out = std::cerr;
    switch(severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            out << "[ERROR] ";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            out << "[ERROR] ";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            out << "[WARNING] ";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            out << "[MSG] ";
            break;
        default:
            out << "[???] ";
            break;
    }
    out << std::string(message, length);
    out << std::endl;
}
