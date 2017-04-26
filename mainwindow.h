#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>

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

private:
    /**
     * @brief  Checks if the model is dirty, then
     *         asks the user if he wants to save his changes.
     * @return If the model can be changed, this function will return true.
     */
    bool ensureModelIsSave();

    /**
     * @brief Saves the current model under the mCurrentFile path
     */
    void save();

    /**
     * @brief Changes the mesh
     * @param model
     */
    void setModel(const Mesh & mesh);

private slots:
    void on_actionUndo_triggered();

    void on_actionQuit_triggered();

    void on_actionSave_triggered();

    void on_actionSave_As_triggered();

    void on_actionNew_triggered();

    void on_actionOpen_triggered();

    void on_mve_modelHasChanged();

    void on_actionRotateRight_triggered();

    void on_actionRotateLeft_triggered();

    void on_actionFlip_Horizontal_triggered();

    void on_actionFlip_Vertical_triggered();

private:
    Ui::MainWindow *ui;
    ModelEditorView *mve;
    TileSetViewer *tse;
    QFileInfo mCurrentFile;
    bool mModelIsDirty;
};

#endif // MAINWINDOW_H
