#ifndef IMEXT_H
#define IMEXT_H

#include "texture.h"

//---- Tip: You can add extra functions within the ImGui:: namespace, here or in your own headers files.
//---- e.g. create variants of the ImGui::Value() helper for your low-level math types, or your own widgets/helpers.
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


#endif // IMEXT_H
