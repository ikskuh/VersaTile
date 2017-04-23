#include "tilesetviewer.h"

TilesetViewer::TilesetViewer(std::shared_ptr<Texture> const & tileset) :
    mTileset(tileset),
    mTileSize(16)
{

}

void TilesetViewer::update(std::string const & name)
{
    ImGui::SetNextWindowContentWidth(this->tileset()->width() * 2 + 8);
    ImGui::SetNextWindowSizeConstraints(
        ImVec2(-1, -1),
        ImVec2(-1, -1));

    if(ImGui::Begin(name.c_str(), nullptr, ImGuiWindowFlags_NoResize))
    {
        auto pos = ImGui::GetCursorScreenPos();
        auto draw = ImGui::GetWindowDrawList();

        auto size = 2.0f * this->tileset()->size();

        ImGui::InvisibleButton("TileSet", size);

        ImVec2 mouse_pos = ImVec2(
            ImGui::GetIO().MousePos.x - pos.x,
            ImGui::GetIO().MousePos.y - pos.y);

        draw->AddImage(
            *this->tileset(),
            pos,
            (glm::vec2)pos + size);
        if(ImGui::IsItemHovered())
        {
            int tx = (int)(mouse_pos.x / 32);
            int ty = (int)(mouse_pos.y / 32);

            ImVec2 points[] =
            {
                pos,
                (glm::vec2)pos + glm::vec2(32.0f, 0.0f),
                (glm::vec2)pos + glm::vec2(32.0f, 32.0f),
                (glm::vec2)pos + glm::vec2(0.0f, 32.0f),
            };
            for(int i = 0; i < 4; i++)
            {
                points[i].x += 32 * tx;
                points[i].y += 32 * ty;
            }
            draw->AddPolyline(
                points,
                4,
                0xFF00FFFF,
                true,
                1.0f,
                true);
        }
    }
    ImGui::End();
}
