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

	void on_mve_modelHasChanged();

private slots:
	void on_actionUndo_triggered();

	void on_actionQuit_triggered();

	void on_actionSave_triggered();

	void on_actionSave_As_triggered();

	void on_actionNew_triggered();

	void on_actionOpen_triggered();

	void on_actionRotateRight_triggered();

	void on_actionRotateLeft_triggered();

	void on_actionFlip_Horizontal_triggered();

	void on_actionFlip_Vertical_triggered();

	void on_actionDelete_Face_triggered();

	void on_toolButtonZoomX1_clicked();

	void on_toolButtonZoomX2_clicked();

	void on_toolButtonZoomX3_clicked();

	void on_toolButtonZoomX4_clicked();

	void on_actionGitHub_Page_triggered();

	void on_actionAbout_triggered();

	void on_actionExport_triggered();

	void on_actionHome_triggered();

	void on_actionSet_Home_triggered();

	void on_actionTop_triggered();

	void on_actionFront_triggered();

	void on_actionSide_triggered();

public:
	virtual void keyPressEvent(QKeyEvent *event) override;

private:
	Ui::MainWindow *ui;
	ModelEditorView *mve;
	TileSetViewer *tse;
	QFileInfo mCurrentFile;
	bool mModelIsDirty;
	QFileInfo mCurrentExport;
	QString mCurrentExportFilter;
};

#endif // MAINWINDOW_H
