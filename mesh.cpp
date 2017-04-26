#include "mesh.h"

#include <QDataStream>

#define EPSILON 0.000001

static bool triangle_intersection(const glm::vec3   V1,  // Triangle vertices
                           const glm::vec3   V2,
                           const glm::vec3   V3,
                           const glm::vec3    O,  //Ray origin
                           const glm::vec3    D,  //Ray direction
                            float* out )
{
    glm::vec3 e1, e2;  //Edge1, Edge2
    glm::vec3 P, Q, T;
    float det, inv_det, u, v;
    float t;

    //Find vectors for two edges sharing V1
    e1 = V2 - V1;
    e2 = V3 - V1;
    //Begin calculating determinant - also used to calculate u parameter
    P = glm::cross(D, e2);
    //if determinant is near zero, ray lies in plane of triangle or ray is parallel to plane of triangle
    det = glm::dot(e1, P);
    //NOT CULLING
    if(det > -EPSILON && det < EPSILON) return 0;
    inv_det = 1.f / det;

    //calculate distance from V1 to ray origin
    T = O - V1;

    //Calculate u parameter and test bound
    u = glm::dot(T, P) * inv_det;
    //The intersection lies outside of the triangle
    if(u < 0.f || u > 1.f) return 0;

    //Prepare to test v parameter
    Q = glm::cross(T, e1);

    //Calculate V parameter and test bound
    v = glm::dot(D, Q) * inv_det;
    //The intersection lies outside of the triangle
    if(v < 0.f || u + v  > 1.f) return 0;

    t = glm::dot(e2, Q) * inv_det;

    if(t > EPSILON) { //ray intersection
        if(out) *out = t;
        return 1;
    }

    // No hit, no win
    return 0;
}

bool Face::intersects(glm::vec3 origin, glm::vec3 direction, float * distance) const
{
    direction = glm::normalize(direction);
    // 0, 1, 3
    bool hit1 = triangle_intersection(
        this->vertices[0].position,
        this->vertices[1].position,
        this->vertices[3].position,
        origin,
        direction,
        distance);
    if(hit1) return true;
    // 0, 2, 3
    return triangle_intersection(
        this->vertices[0].position,
        this->vertices[2].position,
        this->vertices[3].position,
        origin,
        direction,
        distance);
}

void Mesh::save(QFile & target) const
{
    QDataStream io(&target);
    io.setVersion(QDataStream::Qt_5_8);
    io.setByteOrder(QDataStream::LittleEndian);
    io.setFloatingPointPrecision(QDataStream::DoublePrecision);

    io << QString("VERSATILE MESH");
    io << quint8(1) << quint8(0);
    io << this->texture;
    io << quint32(this->faces.size());
    for(size_t i = 0; i < this->faces.size(); i++)
    {
        Face const & face = this->faces[i];
        for(int v = 0; v < 4; v++)
        {
            Vertex const & vertex = face.vertices[v];
            io << vertex.position.x << vertex.position.y << vertex.position.z;
            io << vertex.uv.x << vertex.uv.y;
        }
    }
}

void Mesh::load(QFile & source)
{
    QDataStream io(&source);
    io.setVersion(QDataStream::Qt_5_8);
    io.setByteOrder(QDataStream::LittleEndian);
    io.setFloatingPointPrecision(QDataStream::DoublePrecision);

    QString header;
    quint8 versionMajor, versionMinor;
    io >> header >> versionMajor >> versionMinor;
    if(header != "VERSATILE MESH") {
        qFatal("Invalid file!");
    }
    if(versionMajor != 1 || versionMinor != 0) {
        qFatal("Unsupported file version!");
    }

    io >> this->texture;

    quint32 count;
    io >> count;
    this->faces.clear();
    this->faces.resize(count);
    for(quint32 i = 0; i < count; i++)
    {
        Face & face = this->faces[i];
        for(int v = 0; v < 4; v++)
        {
            Vertex & vertex = face.vertices[v];
            io >> vertex.position.x >> vertex.position.y >> vertex.position.z;
            io >> vertex.uv.x >> vertex.uv.y;
        }
    }
}
