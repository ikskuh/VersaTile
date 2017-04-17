// ImGui SDL2 binding with OpenGL3
// In this binding, ImTextureID is used to store an OpenGL 'GLuint' texture identifier. Read the FAQ about ImTextureID in imgui.cpp.

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

struct SDL_Window;
typedef union SDL_Event SDL_Event;

#include "imgui.h"
#include <SDL2/SDL.h>
#include <GL/gl3w.h>

IMGUI_API bool        ImGui_ImplSdlGL3_Init(SDL_Window* window);
IMGUI_API void        ImGui_ImplSdlGL3_Shutdown();
IMGUI_API void        ImGui_ImplSdlGL3_NewFrame(SDL_Window* window);
IMGUI_API bool        ImGui_ImplSdlGL3_ProcessEvent(SDL_Event* event);

// Use if you want to reset your rendering device without losing ImGui state.
IMGUI_API void        ImGui_ImplSdlGL3_InvalidateDeviceObjects();
IMGUI_API bool        ImGui_ImplSdlGL3_CreateDeviceObjects();

struct GLState
{
    GLint last_program;
    GLint last_texture;
    GLint last_active_texture;
    GLint last_array_buffer;
    GLint last_element_array_buffer;
    GLint last_vertex_array;
    GLint last_blend_src;
    GLint last_blend_dst;
    GLint last_blend_equation_rgb;
    GLint last_blend_equation_alpha;
    GLint last_viewport[4];
    GLint last_scissor_box[4];
    GLboolean last_enable_blend;
    GLboolean last_enable_cull_face;
    GLboolean last_enable_depth_test;
    GLboolean last_enable_scissor_test;

    void restore();

    static GLState backup();
};
