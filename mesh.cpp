#include "mesh.h"

#include <QDataStream>

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
