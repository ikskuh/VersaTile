#include "modeleditorview.h"

#include <QDebug>
#include <QFile>

#include <glm/gtc/matrix_transform.hpp>

static void sysOpenGLDebug(GLenum,GLenum,GLuint,GLenum,GLsizei,const GLchar*,const void *);

ModelEditorView::ModelEditorView(QWidget *parent) :
    QOpenGLWidget(parent),
    mMesh(),
    mTexture(nullptr),
    mPixel(nullptr),
    shader(0),
    vao(0),
    vbuffer(0),
    mOpenGLReady(false)
{
    QSurfaceFormat fmt;
    fmt.setVersion(3, 3);
    this->setFormat(fmt);
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
    this->update();
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
    glm::mat4 matViewProj =
        glm::perspectiveFov(
            glm::radians(60.0f),
            float(this->width()),
            float(this->height()),
            0.1f,
            1000.0f) *
        glm::lookAt(
            glm::vec3(-64, 64, -64),
            glm::vec3(0, 0, 0),
            glm::vec3(0, 1, 0));

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

    glUniformMatrix4fv(this->locMatTransform, 1, GL_FALSE, &matViewProj[0][0]);
    glUniform1i(this->locTexDiffuse, 0);
    glUniform1i(this->locIAlphaTest, 0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

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
                vertices.emplace_back(glm::ivec3(-16 * u, 16 * v, 0));
                vertices.emplace_back(glm::ivec3( 16 * u, 16 * v, 0));

                vertices.emplace_back(glm::ivec3( 16 * u, -16 * v, 0));
                vertices.emplace_back(glm::ivec3( 16 * u,  16 * v, 0));
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
