#ifndef IMEXT_H
#define IMEXT_H

#include "texture.h"
#include <functional>

namespace ImGui
{
    void TooltipText(char const * fmt, ...);

    bool ToolbarButton(Texture const & image, char const * toolTip);

    void OpenGL(ImVec2 size, std::function<void()> render);
}


#endif // IMEXT_H
