#include "modeleditorview.h"

ModelEditorView::ModelEditorView(QWidget *parent) : QOpenGLWidget(parent)
{

}


void ModelEditorView::initializeGL()
{
    glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
}

void ModelEditorView::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void ModelEditorView::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
}
