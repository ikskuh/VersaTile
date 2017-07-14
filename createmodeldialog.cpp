#include "createmodeldialog.h"
#include "ui_createmodeldialog.h"

#include <QDialogButtonBox>
#include <QPushButton>

CreateModelDialog::CreateModelDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateModelDialog)
{
	ui->setupUi(this);
	this->setupButtons();
}

CreateModelDialog::~CreateModelDialog()
{
	delete ui;
}

void CreateModelDialog::on_lineEdit_textChanged(const QString &arg1)
{
	Q_UNUSED(arg1);
	this->setupButtons();
}

void CreateModelDialog::setupButtons()
{
	this->mTilesetImage = QImage(this->ui->lineEdit->text());

	this->ui->buttonBox
		->button( QDialogButtonBox::Ok )
		->setEnabled(!this->mTilesetImage.isNull() );
}
