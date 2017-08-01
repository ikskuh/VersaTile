#ifndef OPENSPRITESHEETDIALOG_H
#define OPENSPRITESHEETDIALOG_H

#include <QDialog>
#include <QGraphicsScene>

namespace Ui {
	class OpenSpritesheetDialog;
}

class OpenSpritesheetDialog : public QDialog
{
	Q_OBJECT

	QImage mSpriteSheet;
	QImage mSourceImage;

	QGraphicsScene * mScene;

public:
	explicit OpenSpritesheetDialog(QWidget *parent = 0);
	~OpenSpritesheetDialog();

	const QImage & spriteSheet() const {
		return this->mSpriteSheet;
	}

	int spriteSize() const;

	void setSpriteSize(int size);

private slots:
	void on_selectFile_clicked();

	void on_fileName_textChanged(const QString &arg1);

	void on_spritesheetMargin_valueChanged(int arg1);

	void on_spritePadding_valueChanged(int arg1);

	void on_spriteSize_valueChanged(int arg1);

private:
	void checkOk();

	void renderImage();

	void updatePreview();

private:
	Ui::OpenSpritesheetDialog *ui;
};

#endif // OPENSPRITESHEETDIALOG_H
