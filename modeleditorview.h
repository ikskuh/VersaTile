#ifndef MODELEDITORVIEW_H
#define MODELEDITORVIEW_H

#include <GL/gl3w.h>
#include <QOpenGLWidget>
#include <QOpenGLTexture>
#include <QScopedPointer>

#include "mesh.h"

class ModelEditorView : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit ModelEditorView(QWidget *parent = 0);

    virtual void initializeGL() override;

    virtual void resizeGL(int w, int h) override;

    virtual void paintGL() override;

    void setMesh(const Mesh & mesh);

    const Mesh & mesh() const { return this->mMesh; }

    Mesh & mesh() { return this->mMesh; }

    bool hasMesh() const
    {
        return !this->mMesh.texture.isNull();
    }

signals:

public slots:
private:
    Mesh mMesh;
    QScopedPointer<QOpenGLTexture> mTexture;
    QScopedPointer<QOpenGLTexture> mPixel;
    GLuint shader, vao, vbuffer;
    bool mOpenGLReady;
    GLint locMatTransform, locTexDiffuse, locVecTint, locIAlphaTest;
};

#endif // MODELEDITORVIEW_H
