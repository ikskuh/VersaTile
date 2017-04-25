#include "modeleditorview.h"

#include <QDebug>
#include <QFile>
#include <QMouseEvent>
#include <QWheelEvent>

#include <glm/gtc/matrix_transform.hpp>

static void sysOpenGLDebug(GLenum,GLenum,GLuint,GLenum,GLsizei,const GLchar*,const void *);

static QDebug operator<<(QDebug debug, const glm::vec3 &c)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << '(' << c.x << "; " << c.y << "; " << c.z << ')';
    return debug;
}
static QDebug operator<<(QDebug debug, const glm::ivec3 &c)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << '(' << c.x << "; " << c.y << "; " << c.z << ')';
    return debug;
}

static inline glm::ivec2 C(const QPoint &c)
{
    return glm::ivec2(c.x(), c.y());
}

ModelEditorView::ModelEditorView(QWidget *parent) :
    QOpenGLWidget(parent),
    mMesh(),
    mTexture(nullptr),
    mPixel(nullptr),
    shader(0),
    vao(0),
    vbuffer(0),
    mOpenGLReady(false),
    mPan(0.0f), mTilt(0.0f), mZoom(128.0f),
    mCameraPosition(), mCameraFocus(), matViewProj(),
    mPlaneAxis(2),
    mSnapToCoarseGrid(true)
{
    QSurfaceFormat fmt;
    fmt.setVersion(3, 3);
    this->setFormat(fmt);
    this->setMouseTracking(true);
    this->grabKeyboard();

    this->mSpriteToInsert = QRect(32, 32, 48, 32);
}

void ModelEditorView::getPlane(glm::ivec3 & normal,glm::ivec3 & tangent, glm::ivec3 & cotangent)
{
    using namespace glm;
    switch(this->mPlaneAxis)
    {
        case 0:
            normal    = ivec3(1, 0, 0);
            tangent   = ivec3(0, 0, 1);
            cotangent = ivec3(0, 1, 0);
            break;
        case 1:
            normal    = ivec3(0, 1, 0);
            tangent   = ivec3(1, 0, 0);
            cotangent = ivec3(0, 0, 1);
            break;
        case 2:
            normal    = ivec3(0, 0, 1);
            tangent   = ivec3(1, 0, 0);
            cotangent = ivec3(0, 1, 0);
            break;
    }
}

void ModelEditorView::keyPressEvent(QKeyEvent *event)
{
    glm::ivec3 normal, tangent, cotangent;
    this->getPlane(normal, tangent, cotangent);

    int stepSize = 1;
    if(this->mSnapToCoarseGrid) {
        stepSize = 16;
    }

    switch(event->key())
    {
        case Qt::Key_W:
            this->mCameraFocus += stepSize * normal;
            break;
        case Qt::Key_S:
            this->mCameraFocus -= stepSize * normal;
            break;
        case Qt::Key_A:
            this->mCameraFocus += stepSize * tangent;
            break;
        case Qt::Key_D:
            this->mCameraFocus -= stepSize * tangent;
            break;
        case Qt::Key_Shift:
            this->mSnapToCoarseGrid = false;
            break;
    }

    this->repaint();
}

void ModelEditorView::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Shift) {
        this->mSnapToCoarseGrid = true;
    }
}

void ModelEditorView::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - this->mLastMouse.x();
    int dy = event->y() - this->mLastMouse.y();
    this->mLastMouse = event->pos();

    if(event->buttons() & Qt::RightButton)
    {
        this->mPan  -= 0.03f * dx;
        this->mTilt -= 0.03f * dy;
        if(this->mTilt <= (-0.99 * M_PI_2)) {
            this->mTilt = (-0.99 * M_PI_2);
        }
        if(this->mTilt >= (0.99 * M_PI_2)) {
            this->mTilt = (0.99 * M_PI_2);
        }
    }
    else
    {
        this->mCursorPosition = this->raycastAgainstPlane(event->x(), event->y());
        if(this->mSnapToCoarseGrid)
        {
            this->mCursorPosition = 16 * (this->mCursorPosition / 16);
        }
    }
    this->repaint();
}

glm::ivec3 ModelEditorView::raycastAgainstPlane(int x, int y) const
{
    // unproject ray here
    glm::vec3 rayStart(
        x,
        this->height() - y - 1,
        0.0f);
    glm::vec3 rayEnd(rayStart.x, rayStart.y, 0.8f);

    glm::vec4 viewport(0, 0, this->width(), this->height());

    rayStart = glm::unProject(
        rayStart,
        glm::mat4(),
        this->matViewProj,
        viewport);
    rayEnd = glm::unProject(
        rayEnd,
        glm::mat4(),
        this->matViewProj,
        viewport);

    glm::vec3 l(glm::normalize(rayEnd - rayStart));
    glm::vec3 l0(rayStart);
    glm::vec3 p0(this->mCameraFocus);
    glm::vec3 n(this->planeNormal());

    float dist = glm::dot(p0 - l0, n) / glm::dot(l, n);

    return glm::ivec3(l0 + dist * l + 0.5f);
}

void ModelEditorView::mousePressEvent(QMouseEvent *event)
{
    mLastMouse = event->pos();

    glm::ivec3 normal, tangent, cotangent;
    this->getPlane(normal, tangent, cotangent);

    if(event->button() == Qt::LeftButton)
    {
        Face face;
        face.vertices[0] = Vertex(
            this->mCursorPosition,
            C(this->mSpriteToInsert.topLeft()));
        face.vertices[1] = Vertex(
            this->mCursorPosition
                + this->mSpriteToInsert.width() * tangent,
            C(this->mSpriteToInsert.topRight()));
        face.vertices[2] = Vertex(
            this->mCursorPosition
                + this->mSpriteToInsert.height() * cotangent,
            C(this->mSpriteToInsert.bottomLeft()));
        face.vertices[3] = Vertex(
            this->mCursorPosition
                + this->mSpriteToInsert.width() * tangent
                + this->mSpriteToInsert.height() * cotangent,
            C(this->mSpriteToInsert.bottomRight()));

        this->mMesh.faces.push_back(face);
    }

    this->repaint();
}

void ModelEditorView::mouseReleaseEvent(QMouseEvent *event)
{

}

void ModelEditorView::wheelEvent(QWheelEvent *event)
{
    this->mZoom -= 0.25f * event->delta();
    if(this->mZoom < 32) {
        this->mZoom = 32;
    }
    this->repaint();
}

void ModelEditorView::setMesh(const Mesh & mesh)
{
    this->mMesh = mesh;

    if(this->mOpenGLReady)
    {
        this->mTexture.reset(new QOpenGLTexture(this->mMesh.texture, QOpenGLTexture::DontGenerateMipMaps));
        this->mTexture->setWrapMode(QOpenGLTexture::ClampToBorder);
        this->mTexture->setBorderColor(1.0f, 0.0f, 1.0f, 1.0f);
        this->mTexture->setMinificationFilter(QOpenGLTexture::Nearest);
        this->mTexture->setMagnificationFilter(QOpenGLTexture::Nearest);
    }
    this->repaint();
}

void ModelEditorView::initializeGL()
{
    if(gl3wInit() < 0) {
        qCritical() << "Failed to initialize OpenGL";
    }

    glDebugMessageCallback(&sysOpenGLDebug, nullptr);
    glEnable(GL_DEBUG_OUTPUT);

    struct { int major, minor; } version;
    glGetIntegerv(GL_MAJOR_VERSION, &version.major);
    glGetIntegerv(GL_MINOR_VERSION, &version.minor);
    qDebug() << "Initialized OpenGL" << version.major << version.minor;

    glClearColor(1.0f, 1.0f, 0.0f, 1.0f);

    glGenBuffers(1, &this->vbuffer);
    glGenVertexArrays(1, &this->vao);

    glBindVertexArray(this->vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, this->vbuffer);

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

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    QFile vshdfile(":/glsl/vertex.glsl");
    if(vshdfile.open(QFile::ReadOnly) == false) {
        qCritical() << "Failed to open vertex shader file";
    }

    QFile fshdfile(":/glsl/fragment.glsl");
    if(fshdfile.open(QFile::ReadOnly) == false) {
        qCritical() << "Failed to open fragment shader file";
    }

    QByteArray vshader = vshdfile.readAll();
    QByteArray fshader = fshdfile.readAll();

    GLchar * vshadersource = vshader.data();
    GLchar * fshadersource = fshader.data();

    this->shader = glCreateProgram();
    GLuint g_VertHandle = glCreateShader(GL_VERTEX_SHADER);
    GLuint g_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(g_VertHandle, 1, &vshadersource, 0);
    glShaderSource(g_FragHandle, 1, &fshadersource, 0);
    glCompileShader(g_VertHandle);
    glCompileShader(g_FragHandle);
    glAttachShader(this->shader, g_VertHandle);
    glAttachShader(this->shader, g_FragHandle);
    glLinkProgram(this->shader);

    this->locMatTransform = glGetUniformLocation(this->shader, "matTransform");
    this->locTexDiffuse   = glGetUniformLocation(this->shader, "texDiffuse");
    this->locVecTint      = glGetUniformLocation(this->shader, "vecTint");
    this->locIAlphaTest   = glGetUniformLocation(this->shader, "iAlphaTest");

    this->mPixel.reset(new QOpenGLTexture(QImage(":/data/pixel.png")));

    if(this->hasMesh())
    {
        this->mTexture.reset(new QOpenGLTexture(this->mMesh.texture, QOpenGLTexture::DontGenerateMipMaps));
        this->mTexture->setWrapMode(QOpenGLTexture::ClampToBorder);
        this->mTexture->setBorderColor(1.0f, 0.0f, 1.0f, 1.0f);
        this->mTexture->setMinificationFilter(QOpenGLTexture::Nearest);
        this->mTexture->setMagnificationFilter(QOpenGLTexture::Nearest);
    }
}

void ModelEditorView::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void ModelEditorView::paintGL()
{
    glm::vec3 cameraOffset(
        sin(this->mPan) * cos(this->mTilt),
        sin(this->mTilt),
        cos(this->mPan) * cos(this->mTilt));

    this->mCameraPosition = glm::vec3(this->mCameraFocus) - this->mZoom * cameraOffset;

    auto matProj = glm::perspectiveFov(
        glm::radians(60.0f),
        float(this->width()),
        float(this->height()),
        0.1f,
        1000.0f);
    auto matView = glm::lookAt(
        this->mCameraPosition,
        glm::vec3(this->mCameraFocus),
        glm::vec3(0, 1, 0));

    this->matViewProj = matProj * matView;

    // glm::mat4 matInvViewProj = glm::inverse(matViewProj);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    std::vector<Vertex> vertices;

    glBindVertexArray(this->vao);
    glUseProgram(this->shader);
    glActiveTexture(GL_TEXTURE0);

    glUniformMatrix4fv(this->locMatTransform, 1, GL_FALSE, &this->matViewProj[0][0]);
    glUniform1i(this->locTexDiffuse, 0);
    glUniform1i(this->locIAlphaTest, 0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glm::ivec3 normal, tangent, cotangent;
    this->getPlane(normal, tangent, cotangent);

    { // Render a ground plane
        glDepthMask(GL_TRUE);
        this->mPixel->bind();

        vertices.clear();
        vertices.emplace_back(glm::ivec3(-256, 0, -256));
        vertices.emplace_back(glm::ivec3( 256, 0, -256));
        vertices.emplace_back(glm::ivec3(-256, 0,  256));
        vertices.emplace_back(glm::ivec3( 256, 0,  256));

        glBindBuffer(GL_ARRAY_BUFFER, this->vbuffer);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(Vertex) * vertices.size(),
            vertices.data(),
            GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glUniform1i(this->locIAlphaTest, 0);
        glUniform4f(this->locVecTint, 0.28f, 0.54f, 0.28f, 1.0f);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size());
    }

    { // Render the fitting grid
        glDepthMask(GL_TRUE);
        this->mPixel->bind();

        vertices.clear();
        for(int u = -10; u <= 10; u++)
        {
            for(int v = -10; v <= 10; v++)
            {
                vertices.emplace_back(this->mCameraFocus + 16 * u * tangent + 16 * v * cotangent);
                vertices.emplace_back(this->mCameraFocus + 16 * u * tangent - 16 * v * cotangent);

                vertices.emplace_back(this->mCameraFocus + 16 * u * cotangent + 16 * v * tangent);
                vertices.emplace_back(this->mCameraFocus + 16 * u * cotangent - 16 * v * tangent);
            }
        }

        glBindBuffer(GL_ARRAY_BUFFER, this->vbuffer);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(Vertex) * vertices.size(),
            vertices.data(),
            GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glUniform1i(this->locIAlphaTest, 0);
        glUniform4f(this->locVecTint, 1.0f, 1.0f, 1.0f, 0.6f);

        glDrawArrays(GL_LINES, 0, vertices.size());
    }


    if((this->mMesh.faces.size() > 0) && (this->mTexture != nullptr))
    { // Render the 3D model
        glDepthMask(GL_TRUE);
        this->mTexture->bind();
        vertices.clear();
        vertices.reserve(this->mMesh.faces.size() * 6);
        for(Face & face : this->mMesh.faces)
        {
            vertices.push_back(face.vertices[0]);
            vertices.push_back(face.vertices[1]);
            vertices.push_back(face.vertices[3]);

            vertices.push_back(face.vertices[0]);
            vertices.push_back(face.vertices[2]);
            vertices.push_back(face.vertices[3]);
        }

        if(this->mSpriteToInsert.width() > 0)
        { // The last sprite is the currently inserted sprite.

            Vertex face[] =
            {
                Vertex(this->mCursorPosition, C(this->mSpriteToInsert.topLeft())),
                Vertex(this->mCursorPosition + this->mSpriteToInsert.width() * tangent, C(this->mSpriteToInsert.topRight())),
                Vertex(this->mCursorPosition + this->mSpriteToInsert.height() * cotangent, C(this->mSpriteToInsert.bottomLeft())),
                Vertex(this->mCursorPosition + this->mSpriteToInsert.width() * tangent + this->mSpriteToInsert.height() * cotangent, C(this->mSpriteToInsert.bottomRight())),
            };

            vertices.push_back(face[0]);
            vertices.push_back(face[1]);
            vertices.push_back(face[3]);

            vertices.push_back(face[0]);
            vertices.push_back(face[2]);
            vertices.push_back(face[3]);
        }

        glBindBuffer(GL_ARRAY_BUFFER, this->vbuffer);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(Vertex) * vertices.size(),
            vertices.data(),
            GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glUniform1i(this->locIAlphaTest, 1);
        glUniform4f(this->locVecTint, 1.0f, 1.0f, 1.0f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    }

    glDepthMask(GL_TRUE);
}

static void sysOpenGLDebug(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar * message,
    const void *userParam)
{
    Q_UNUSED(source);
    Q_UNUSED(type);
    Q_UNUSED(id);
    Q_UNUSED(userParam);
    QString msg = QString::fromUtf8(message, length);
    switch(severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            qCritical() << msg;
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            qCritical() << msg;
            break;
        case GL_DEBUG_SEVERITY_LOW:
            qCritical() << msg;
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            qInfo() << msg;
            break;
        default:
            qDebug() << msg;
            break;
    }
}
