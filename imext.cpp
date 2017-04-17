#include "imext.h"
#include "glstate.h"
#include <stdarg.h>
#include <memory>

namespace ImGui
{
    void TooltipText(char const * fmt, ...)
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

    bool ToolbarButton(Texture const & image, char const * toolTip)
    {
        auto result = ImGui::ImageButton(image, image.size());
        ImGui::TooltipText(toolTip);
        ImGui::SameLine();
        return result;
    }

    void OpenGL(ImVec2 size, std::function<void()> render)
    {
        struct metainfo
        {
            ImVec2 pos;
            ImVec2 size;
            std::function<void()> renderer;
        };

        metainfo * info = new metainfo;
        info->pos = ImGui::GetCursorScreenPos();
        info->size = size;
        info->renderer = render;

        ImGui::GetWindowDrawList()->AddCallback([](const ImDrawList*, const ImDrawCmd* cmd)
        {
            auto backup = GLState::backup();

            std::unique_ptr<metainfo> info((metainfo*)cmd->UserCallbackData);
            int x,y,w,h;

            x = info->pos.x;
            y = info->pos.y;
            w = info->size.x;
            h = info->size.y;

            if(x < 0 || y < 0 || w <= 0 || h <= 0)
                return;

            // FIX: When larger screen, everything breaks!

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

            if(x < 0 || y < 0 || w <= 0 || h <= 0)
                return;

            glScissor(x, (int)(ImGui::GetIO().DisplaySize.y - y - h), w, h);

            // TODO: Do OpenGL rendering here
            if(info->renderer)
                info->renderer();

            // Restore the OpenGL state
            backup.restore();
        }, info);
        ImGui::InvisibleButton("glwindow", info->size);
    }
}
