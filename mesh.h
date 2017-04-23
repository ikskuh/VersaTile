#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <vector>

struct Vertex
{
    Vertex() : position(), uv() { }

    Vertex(glm::ivec3 vec) : position(vec), uv() { }

    Vertex(glm::ivec3 vec, glm::ivec2 uv) : position(vec), uv(uv) { }

    glm::ivec3 position;
    glm::ivec2 uv;
};

struct Face
{
    Vertex vertices[4];
};

using Mesh = std::vector<Face>;

#endif // MESH_H
