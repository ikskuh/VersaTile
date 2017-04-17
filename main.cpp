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

GLuint shader, vao;

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


    {
        GLuint vertexBuffer;

        float vertices[] =
        {
            -0.8f, -0.8f,
             0.8f, -0.8f,
             0.8f,  0.8f,
        };

        glGenBuffers(1, &vertexBuffer);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof vertices,
            vertices,
            GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);


        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

        glVertexAttribPointer(
            0,
            2,
            GL_FLOAT,
            GL_FALSE,
            2 * sizeof(float),
            nullptr);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        const GLchar *vertex_shader =
            "#version 330\n"
            "layout(location = 0) in vec2 Position;\n"
            "out vec2 Frag_UV;\n"
            "void main()\n"
            "{\n"
            "	Frag_UV = Position;\n"
            "	gl_Position = vec4(Position.xy,0,1);\n"
            "}\n";

        const GLchar* fragment_shader =
            "#version 330\n"
            "in vec2 Frag_UV;\n"
            "out vec4 Out_Color;\n"
            "void main()\n"
            "{\n"
            "	Out_Color = vec4(fract(Frag_UV.xy), 0.0f, 1.0f);\n"
            "}\n";

        shader = glCreateProgram();
        GLuint g_VertHandle = glCreateShader(GL_VERTEX_SHADER);
        GLuint g_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(g_VertHandle, 1, &vertex_shader, 0);
        glShaderSource(g_FragHandle, 1, &fragment_shader, 0);
        glCompileShader(g_VertHandle);
        glCompileShader(g_FragHandle);
        glAttachShader(shader, g_VertHandle);
        glAttachShader(shader, g_FragHandle);
        glLinkProgram(shader);
    }


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

        if(ImGui::Begin("Render Window"))
        {
            ImGui::Text("Hello, rendering!");

            auto * list = ImGui::GetWindowDrawList();
            struct {
                ImVec2 pos;
                ImVec2 size;
            } info;
            info.pos = ImGui::GetCursorScreenPos();
            info.size = ImGui::GetContentRegionAvail(); // ImVec2(ImGui::GetContentRegionAvailWidth(), 100);
            info.size.x = std::max(info.size.x, 400.0f);
            info.size.y = std::max(info.size.y, 300.0f);

            list->AddCallback([](const ImDrawList* parent_list, const ImDrawCmd* cmd)
            {
                auto backup = GLState::backup();

                auto i = (typeof info *)cmd->UserCallbackData;
                int x,y,w,h;

                x = i->pos.x;
                y = i->pos.y;
                w = i->size.x;
                h = i->size.y;

                // Setup viewport before scissor rect clipping
                glViewport(x, (int)(ImGui::GetIO().DisplaySize.y - y - h), w, h);

                // Getting the scissor rectangle right (clip against outer bounds)
                if(x < cmd->ClipRect.x)
                {
                    w -= cmd->ClipRect.x - x;
                    x = cmd->ClipRect.x;
                }
                if(y < cmd->ClipRect.y)
                {
                    h -= cmd->ClipRect.y - y;
                    y = cmd->ClipRect.y;
                }

                if(x+w > cmd->ClipRect.z)
                    w = cmd->ClipRect.z - x;
                if(y+h > cmd->ClipRect.w)
                    h = cmd->ClipRect.w - y;

                glScissor(x, (int)(ImGui::GetIO().DisplaySize.y - y - h), w, h);
                glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);


                // TODO: Do OpenGL rendering here
                glBindVertexArray(vao);
                glUseProgram(shader);
                glDrawArrays(GL_TRIANGLES, 0, 3);


                // Restore the OpenGL state
                backup.restore();

            }, &info);
            ImGui::InvisibleButton("glwindow", info.size);
            if(ImGui::IsItemHovered())
            {
                ImVec2 mouse_pos = ImVec2(
                    ImGui::GetIO().MousePos.x - info.pos.x,
                    ImGui::GetIO().MousePos.y - info.pos.y);
                // syslog::message("hovered at (", mouse_pos.x, ",", mouse_pos.y, ")");
            }

            ImGui::Text("Good bye, rendering!");

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
