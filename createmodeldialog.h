#ifndef CREATEMODELDIALOG_H
#define CREATEMODELDIALOG_H

#include <QDialog>
#include <QGraphicsScene>

namespace Ui {
	class CreateModelDialog;
}

class CreateModelDialog : public QDialog
{
	Q_OBJECT

	QImage mTilesetImage;
	QImage mOriginalTilesetImage;

	QGraphicsScene * mScene;
	QGraphicsPixmapItem * mLastPixmap;
public:
	explicit CreateModelDialog(QWidget *parent = 0);
	~CreateModelDialog();

	const QImage & getTileSet() const {
		return this->mTilesetImage;
	}

	int getMinimumTileSize() const;

private:
	void checkOk();

	void renderImage();

private slots:
	void on_lineEdit_textChanged(const QString &arg1);

	void setupButtons();

	void on_tileSize_valueChanged(int arg1);

	void on_tilesetMargin_valueChanged(int arg1);

	void on_tilePadding_valueChanged(int arg1);

private:
	Ui::CreateModelDialog *ui;
};

#endif // CREATEMODELDIALOG_H
