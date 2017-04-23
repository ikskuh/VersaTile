#include "mesheditor.h"

#include <glm/gtc/matrix_transform.hpp>

#include "imgui.h"
#include "imext.h"
#include "system.h"

MeshEditor::MeshEditor(std::shared_ptr<Texture> const & texture)
    : mMesh(), mTexture(texture)
{

}

void MeshEditor::update(std::string const & name)
{
    ImGui::SetNextWindowSize(ImVec2(480, 320), ImGuiSetCond_FirstUseEver);
    if(ImGui::Begin(name.c_str()))
    {
        auto size = ImGui::GetContentRegionAvail();
        auto pos = ImGui::GetCursorScreenPos();

        this->matViewProj =
            glm::perspectiveFov(glm::radians(60.0f), size.x, size.y, 0.1f, 1000.0f) *
            glm::lookAt(
                glm::vec3(0, 1, -2),
                glm::vec3(0, 0, 0),
                glm::vec3(0, 1, 0));

        this->matInvViewProj = glm::inverse(this->matViewProj);

        ImGui::OpenGL(size, [this]()
        {
            glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glBindBuffer(GL_ARRAY_BUFFER, sys::vbuffer);

            std::vector<Vertex> vertices;
            vertices.reserve(this->mesh().size() * 6);
            for(Face & face : this->mesh())
            {
                vertices.push_back(face.vertices[0]);
                vertices.push_back(face.vertices[1]);
                vertices.push_back(face.vertices[3]);

                vertices.push_back(face.vertices[0]);
                vertices.push_back(face.vertices[2]);
                vertices.push_back(face.vertices[3]);
            }
            glBufferData(
                GL_ARRAY_BUFFER,
                sizeof(Vertex) * vertices.size(),
                vertices.data(),
                GL_STATIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindVertexArray(sys::vao);
            glUseProgram(sys::shader);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, this->mTexture->id());

            glUniformMatrix4fv(glGetUniformLocation(sys::shader, "matTransform"), 1, GL_FALSE, &matViewProj[0][0]);
            glUniform1i(glGetUniformLocation(sys::shader, "texDiffuse"), 0);

            glDrawArrays(GL_TRIANGLES, 0, vertices.size());
        });
        if(ImGui::IsItemHovered())
        {
            ImVec2 mouse_pos = ImVec2(
                ImGui::GetIO().MousePos.x - pos.x,
                ImGui::GetIO().MousePos.y - pos.y);
        }

    }
    ImGui::End();
}
