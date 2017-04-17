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
#include "system.h"
#include "texture.h"
#include "imext.h"
#include "iconstorage.h"
#include "syslog.h"

static void sysOpenGLDebug(GLenum,GLenum,GLuint,GLenum,GLsizei,const GLchar*,const void *);

int main(int, char**)
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        syslog::error("Could not initialize SDL: ", SDL_GetError());
        return -1;
    }
    if(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == 0)
    {
        syslog::error("Could not initialize SDL2_image: ", IMG_GetError());
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

    // Initialize icons
    Icons = new IconStorage;

    // Setup ImGui binding
    ImGui_ImplSdlGL3_Init(window);


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
            if(ImGui::ToolbarButton(Icons->Open, "Open file")) sys::openFile();
            if(ImGui::ToolbarButton(Icons->Save, "Save file"))
            {

            }
            if(ImGui::ToolbarButton(Icons->Close, "Close file"))
            {

            }
            ImGui::End();
        }

        static bool aboutIsOpen = false;
        if(ImGui::BeginMainMenuBar())
        {
            if(ImGui::BeginMenu("File"))
            {
                if(ImGui::MenuItem("Open")) sys::openFile();
                ImGui::Separator();
                if(ImGui::MenuItem("Quit"))
                {
                    done = true;
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Help"))
            {
                aboutIsOpen |= ImGui::MenuItem("About");
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        ImGui::SetNextWindowPosCenter(ImGuiSetCond_Appearing);
        if(aboutIsOpen && ImGui::Begin("About Versa-Tile", &aboutIsOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize))
        {
            char const * aboutContents =
R"raw(This editor provides a simple editing interface to
create models based on a tile set.

Its UI is written with dear imgui, an immediate gui framework
written by ocornut.

The idea of this mode of model editing was inspired by
Crocotile and a blender plugin that works similar.)raw";
            ImGui::Text(aboutContents);
            ImGui::Separator();

            if (ImGui::Button("Close")) {
                aboutIsOpen = false;
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
    std::string msg(message, length);

    switch(severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            syslog::error(msg);
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            syslog::error(msg);
            break;
        case GL_DEBUG_SEVERITY_LOW:
            syslog::warning(msg);
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            syslog::message(msg);
            break;
        default:
            syslog::message(msg);
            break;
    }
}
