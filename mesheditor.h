#ifndef MESHEDITOR_H
#define MESHEDITOR_H

#include "mesh.h"
#include "texture.h"
#include <string>
#include <memory>

class MeshEditor
{
private:
    Mesh mMesh;
    std::shared_ptr<Texture> mTexture;
    glm::mat4 matViewProj;
    glm::mat4 matInvViewProj;
public:
    explicit MeshEditor(std::shared_ptr<Texture> const & texture);

    void update(std::string const & name);

    Mesh & mesh() { return this->mMesh; }
};

#endif // MESHEDITOR_H
