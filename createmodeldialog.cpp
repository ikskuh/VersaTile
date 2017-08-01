#include "createmodeldialog.h"
#include "ui_createmodeldialog.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QSpinBox>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QDebug>
#include <QPainter>
#include <QGraphicsPixmapItem>

CreateModelDialog::CreateModelDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateModelDialog)
{
	ui->setupUi(this);
	this->mLastPixmap = nullptr;
	this->mScene = new QGraphicsScene(this);
	this->ui->preview->setScene(this->mScene);
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
	this->mOriginalTilesetImage = QImage(this->ui->lineEdit->text());

	if(this->mOriginalTilesetImage.isNull() == false) {
		auto limit = qMin(
			this->mOriginalTilesetImage.width(),
			this->mOriginalTilesetImage.height());
		this->ui->tileSize->setMaximum(limit);
		this->ui->tilePadding->setMaximum(limit / 2);
		this->ui->tilesetMargin->setMaximum(limit / 2);

		this->renderImage();
	}

	this->checkOk();
}


void CreateModelDialog::checkOk()
{
	this->ui->buttonBox
		->button( QDialogButtonBox::Ok )
		->setEnabled(!this->mTilesetImage.isNull() );
}

void CreateModelDialog::renderImage()
{
	if(this->mOriginalTilesetImage.isNull()) {
		this->mTilesetImage = QImage();
		this->checkOk();
		return;
	}
	int margin = this->ui->tilesetMargin->value();
	int padding = this->ui->tilePadding->value();
	int size = this->ui->tileSize->value();

	qDebug() << size << margin << padding;

	int numX = (this->mOriginalTilesetImage.width() - 2 * margin) / (size + padding);
	int numY = (this->mOriginalTilesetImage.height() - 2 * margin) / (size + padding);

	qDebug() << numX << numY;

	if(numX <= 0 || numY <= 0) {
		this->mTilesetImage = QImage();
		this->checkOk();
		return;
	}
	this->mTilesetImage = QImage(numX * size, numY * size, this->mOriginalTilesetImage.format());
	{
		QPainter painter(&this->mTilesetImage);
		painter.fillRect(this->mTilesetImage.rect(), QBrush(QColor(0,0,0,0)));
		painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
		for(int y = 0; y < numY; y++)
		{
			for(int x = 0; x < numX; x++)
			{
				painter.drawImage(
					QRect(size * x, size * y, size, size),
					this->mOriginalTilesetImage,
					QRect(
						margin + (size + padding) * x,
						margin + (size + padding) * y,
						size,
						size));
			}
		}
	}

	this->mScene->clear();
	this->mScene->setSceneRect(QRect(0, 0, numX * size, numY * size));
	this->mScene->addPixmap(QPixmap::fromImage(this->mTilesetImage));

	this->ui->preview->resetCachedContent();
	this->ui->preview->updateSceneRect(this->mScene->sceneRect());

	this->checkOk();
}

int CreateModelDialog::getMinimumTileSize() const
{
	return this->ui->tileSize->value();
}

void CreateModelDialog::on_tileSize_valueChanged(int arg1)
{
	this->renderImage();
}

void CreateModelDialog::on_tilesetMargin_valueChanged(int arg1)
{
	this->renderImage();
}

void CreateModelDialog::on_tilePadding_valueChanged(int arg1)
{
	this->renderImage();
}
