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

    void setPlaneAxis(int axis)
    {
        this->mPlaneAxis = axis % 3;
        this->repaint();
    }

    glm::ivec3 planeNormal() const
    {
        glm::ivec3 normal(0,0,0);
        normal[this->mPlaneAxis % 3] = 1;
        return normal;
    }

    glm::ivec3 raycastAgainstPlane(int x, int y) const;

public:

    virtual void mouseMoveEvent(QMouseEvent *event) override;

    virtual void mousePressEvent(QMouseEvent *event) override;

    virtual void mouseReleaseEvent(QMouseEvent *event) override;

    virtual void wheelEvent(QWheelEvent *event) override;

    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
signals:

public slots:
private:
    Mesh mMesh;
    QScopedPointer<QOpenGLTexture> mTexture;
    QScopedPointer<QOpenGLTexture> mPixel;
    GLuint shader, vao, vbuffer;
    bool mOpenGLReady;
    GLint locMatTransform, locTexDiffuse, locVecTint, locIAlphaTest;
    QPoint mLastMouse;
    float mPan, mTilt, mZoom;
    glm::vec3 mCameraPosition;
    glm::ivec3 mCameraFocus;
    glm::mat4 matViewProj;
    int mPlaneAxis;
    QRect mSpriteToInsert;
    glm::ivec3 mCursorPosition;
    bool mSnapToCoarseGrid;
};

#endif // MODELEDITORVIEW_H
