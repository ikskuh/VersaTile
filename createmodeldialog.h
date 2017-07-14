#ifndef CREATEMODELDIALOG_H
#define CREATEMODELDIALOG_H

#include <QDialog>

namespace Ui {
	class CreateModelDialog;
}

class CreateModelDialog : public QDialog
{
	Q_OBJECT

	QImage mTilesetImage;

public:
	explicit CreateModelDialog(QWidget *parent = 0);
	~CreateModelDialog();

	const QImage & getTileSet() const {
		return this->mTilesetImage;
	}

	int getMinimumTileSize() const;

private slots:
	void on_lineEdit_textChanged(const QString &arg1);

	void setupButtons();

private:
	Ui::CreateModelDialog *ui;
};

#endif // CREATEMODELDIALOG_H
