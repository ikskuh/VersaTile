#include <nfd.h>
#include <iostream>
#include <libgen.h>
#include <string.h>
#include <vector>
#include <memory>
#include <cstddef>
#include <fstream>

#include "imgui.h"
#include "imext.h"
#include "syslog.h"
#include "json.hpp"

#include "system.h"
#include "mesheditor.h"
#include "tilesetviewer.h"

namespace sys
{
    static nfdchar_t * workingDirectory = nullptr;
    static std::vector<std::unique_ptr<EditorWindow>> editors;
    static std::shared_ptr<Texture> texture;

    GLuint shader, vao, vbuffer;

    std::unique_ptr<Texture> pixel;

    void init()
    {
        pixel = std::unique_ptr<Texture>(new Texture("data/pixel.png"));
        texture = std::make_shared<Texture>("data/tilesets/croco.png");

        glGenBuffers(1, &vbuffer);
        /*
        glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof vertices,
            vertices,
            GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        */

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, vbuffer);

        glVertexAttribPointer(
            0,
            3,
            GL_INT,
            GL_FALSE,
            sizeof(Vertex),
            (void const * )offsetof(Vertex, position));
        glVertexAttribPointer(
            1,
            2,
            GL_INT,
            GL_FALSE,
            sizeof(Vertex),
            (void const * )offsetof(Vertex, uv));

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        const GLchar *vertex_shader =
R"glsl(#version 330
//enable GL_ARB_explicit_attrib_location
layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 UV;
uniform mat4 matTransform;
out vec2 Frag_UV;
void main()
{
    Frag_UV = UV;
    gl_Position = matTransform * vec4(Position.xyz,1);
    // gl_Position = vec4(Position.xz, 0, 1);
})glsl";

        const GLchar* fragment_shader =
R"glsl(#version 330
in vec2 Frag_UV;
out vec4 Out_Color;
uniform sampler2D texDiffuse;
uniform vec4 vecTint;
void main()
{
    ivec2 size = textureSize(texDiffuse, 0);
    vec2 uv = vec2(
        Frag_UV.x / float(size.x),
        Frag_UV.y / float(size.y));
    Out_Color = vecTint * texture(texDiffuse, uv);
})glsl";

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

    void update()
    {
        for(std::unique_ptr<EditorWindow> & edit : editors)
        {
            std::string name("Unknown Editor##");
            if(dynamic_cast<MeshEditor*>(edit.get())) {
                name = "Mesh Editor##";
            }
            else if(dynamic_cast<TilesetViewer*>(edit.get())) {
                name = "Tile Set##";
            }
            name += std::to_string((uintptr_t)edit.get());
            edit->update(name);
        }
    }

    void newFile()
    {
        std::unique_ptr<MeshEditor> editor(new MeshEditor(texture));

        editor->mesh().push_back(Face
        {
            {
                Vertex(glm::ivec3( 0,  0,  0), glm::ivec2(208+0, 80+ 0)),
                Vertex(glm::ivec3(32,  0,  0), glm::ivec2(208+32,80+ 0)),
                Vertex(glm::ivec3( 0, 32,  0), glm::ivec2(208+0, 80+32)),
                Vertex(glm::ivec3(32, 32,  0), glm::ivec2(208+32,80+32)),
            }
        });

        editors.emplace_back(std::move(editor));
    }

    void openFile()
    {
        nfdchar_t * path;
        nfdchar_t const * filters = "json";
        auto result = NFD_OpenDialog(filters, workingDirectory, &path);
        switch(result)
        {
            case NFD_OKAY:
                if(workingDirectory != nullptr) {
                    free(workingDirectory);
                }
                workingDirectory = strdup(path);
                dirname(workingDirectory);

                openFile(path);

                free(path);
                break;
            case NFD_CANCEL:
                break;
            case NFD_ERROR:
                std::cerr << "[ERR] " << NFD_GetError() << std::endl;
                break;
        }
    }

    void openFile(char const * fileName)
    {
        using namespace nlohmann;
        std::cout << "Open file '" << fileName << "'" << std::endl;

        std::ifstream input(fileName);
        if(!input.is_open()) {
            syslog::error("Failed to open file '", fileName, "': Not found.");
            return;
        }

        json file;
        try {
            input >> file;
        }
        catch(...) {
            syslog::error("Failed to open file '", fileName, "': Not json");
            return;
        }

        if(file["versatile"].is_null() || !file["versatile"]) {
            syslog::error("Failed to open file '", fileName, "': File not versatile *hrhr*");
            return;
        }

        if(file["type"] == "tileset")
        {
            editors.emplace_back(new TilesetViewer(texture));
        }
        else if(file["type"] == "model")
        {
            editors.emplace_back(new MeshEditor(texture));
        }
        else
        {
            syslog::error("Failed to open file '", fileName, "': Not supported.");
        }
    }
}
