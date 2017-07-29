#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include <QSettings>

namespace Ui {
	class OptionsDialog;
}

class OptionsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit OptionsDialog(QWidget *parent = 0);
	~OptionsDialog();

private slots:
	void on_buttonBox_accepted();

private:
	Ui::OptionsDialog *ui;
	QSettings mSettings;
};

#endif // OPTIONSDIALOG_H
