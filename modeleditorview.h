#ifndef MODELEDITORVIEW_H
#define MODELEDITORVIEW_H

#include <QOpenGLWidget>

class ModelEditorView : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit ModelEditorView(QWidget *parent = 0);

    virtual void initializeGL() override;

    virtual void resizeGL(int w, int h) override;

    virtual void paintGL() override;

signals:

public slots:
};

#endif // MODELEDITORVIEW_H
