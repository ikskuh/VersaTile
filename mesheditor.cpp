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
                glm::vec3(-64, 32, -256),
                glm::vec3(0, 0, 0),
                glm::vec3(0, 1, 0));

        this->matInvViewProj = glm::inverse(this->matViewProj);

        ImGui::OpenGL(size, [this]()
        {
            glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
            glClearDepth(1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


            std::vector<Vertex> vertices;

            glBindVertexArray(sys::vao);
            glUseProgram(sys::shader);
            glActiveTexture(GL_TEXTURE0);

            glUniformMatrix4fv(glGetUniformLocation(sys::shader, "matTransform"), 1, GL_FALSE, &matViewProj[0][0]);
            glUniform1i(glGetUniformLocation(sys::shader, "texDiffuse"), 0);

            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);

            { // Render a ground plane
                glDepthMask(GL_TRUE);
                glBindTexture(GL_TEXTURE_2D, sys::pixel->id());

                vertices.clear();
                vertices.emplace_back(glm::ivec3(-256, 0, -256));
                vertices.emplace_back(glm::ivec3( 256, 0, -256));
                vertices.emplace_back(glm::ivec3(-256, 0,  256));
                vertices.emplace_back(glm::ivec3( 256, 0,  256));

                glBindBuffer(GL_ARRAY_BUFFER, sys::vbuffer);
                glBufferData(
                    GL_ARRAY_BUFFER,
                    sizeof(Vertex) * vertices.size(),
                    vertices.data(),
                    GL_STATIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glUniform4f(glGetUniformLocation(sys::shader, "vecTint"), 0.28f, 0.54f, 0.28f, 1.0f);

                glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size());
            }

            { // Render the fitting grid
                glDepthMask(GL_TRUE);
                glBindTexture(GL_TEXTURE_2D, sys::pixel->id());

                vertices.clear();
                for(int u = -10; u <= 10; u++)
                {
                    for(int v = -10; v <= 10; v++)
                    {
                        vertices.emplace_back(glm::ivec3(-16 * u, 16 * v, 0));
                        vertices.emplace_back(glm::ivec3( 16 * u, 16 * v, 0));

                        vertices.emplace_back(glm::ivec3( 16 * u, -16 * v, 0));
                        vertices.emplace_back(glm::ivec3( 16 * u,  16 * v, 0));
                    }
                }

                glBindBuffer(GL_ARRAY_BUFFER, sys::vbuffer);
                glBufferData(
                    GL_ARRAY_BUFFER,
                    sizeof(Vertex) * vertices.size(),
                    vertices.data(),
                    GL_STATIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glUniform4f(glGetUniformLocation(sys::shader, "vecTint"), 1.0f, 1.0f, 1.0f, 0.6f);

                glDrawArrays(GL_LINES, 0, vertices.size());
            }

            { // Render the 3D model
                glDepthMask(GL_TRUE);
                glBindTexture(GL_TEXTURE_2D, this->mTexture->id());
                vertices.clear();
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
                glBindBuffer(GL_ARRAY_BUFFER, sys::vbuffer);
                glBufferData(
                    GL_ARRAY_BUFFER,
                    sizeof(Vertex) * vertices.size(),
                    vertices.data(),
                    GL_STATIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glUniform4f(glGetUniformLocation(sys::shader, "vecTint"), 1.0f, 1.0f, 1.0f, 1.0f);
                glDrawArrays(GL_TRIANGLES, 0, vertices.size());
            }

            glDepthMask(GL_TRUE);
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
