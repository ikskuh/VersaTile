#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "modeleditorview.h"
#include "tilesetviewer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionPlane_Ground_XZ_triggered();

    void on_actionPlane_Front_XY_triggered();

    void on_actionPlane_Side_YZ_triggered();

    void on_actionUndo_triggered();

    void on_actionQuit_triggered();

private:
    Ui::MainWindow *ui;
    ModelEditorView *mve;
    TileSetViewer *tse;
};

#endif // MAINWINDOW_H
