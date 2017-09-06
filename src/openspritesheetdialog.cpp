#include "openspritesheetdialog.h"
#include "ui_openspritesheetdialog.h"

#include <QFileDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QDebug>
#include <QGraphicsPixmapItem>

OpenSpritesheetDialog::OpenSpritesheetDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenSpritesheetDialog)
{
	ui->setupUi(this);

	this->mScene = new QGraphicsScene(this);
	this->ui->preview->setScene(this->mScene);
	this->checkOk();
}


void OpenSpritesheetDialog::checkOk()
{
	this->ui->buttonBox
		->button( QDialogButtonBox::Ok )
		->setEnabled(!this->mSpriteSheet.isNull() );
}

QImage OpenSpritesheetDialog::renderImage(QImage const & src, int size, int padding, int margin)
{
	if(src.isNull()) {
		return src;
	}

	int numX = (src.width() - 2 * margin) / (size + padding);
	int numY = (src.height() - 2 * margin) / (size + padding);

	if(numX <= 0 || numY <= 0) {
		return QImage();
	}
	QImage dest(numX * size, numY * size, src.format());
	dest.fill(qRgba(0, 0, 0, 0));
	{
		QPainter painter(&dest);
		painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
		for(int y = 0; y < numY; y++)
		{
			for(int x = 0; x < numX; x++)
			{
				painter.drawImage(
					QRect(size * x, size * y, size, size),
					src,
					QRect(
						margin + (size + padding) * x,
						margin + (size + padding) * y,
						size,
						size));
			}
		}
	}
	return dest;
}

void OpenSpritesheetDialog::renderImage()
{
	int margin = this->ui->spritesheetMargin->value();
	int padding = this->ui->spritePadding->value();
	int size = this->ui->spriteSize->value();

	this->mSpriteSheet = renderImage(
		this->mSourceImage,
		size,
		padding,
		margin);
}

OpenSpritesheetDialog::~OpenSpritesheetDialog()
{
	delete ui;
}

void OpenSpritesheetDialog::on_selectFile_clicked()
{
	QString file = QFileDialog::getOpenFileName(
		this,
		"Select the spritesheet image you want to use",
		this->ui->fileName->text(),
		"Image Files (*.png *.jpg *.bmp)");
	if(file.isNull()) {
		return;
	}
	this->ui->fileName->setText(file);
}


void OpenSpritesheetDialog::setSpriteSize(int size)
{
	this->ui->spriteSize->setValue(size);
}

int OpenSpritesheetDialog::spriteSize() const
{
	return this->ui->spriteSize->value();
}

void OpenSpritesheetDialog::updatePreview()
{
	this->mScene->clear();
	if(this->mSpriteSheet.isNull() == false)
	{
		// this->mScene->setSceneRect(this->mSpriteSheet.rect());

		int size = this->ui->spriteSize->value();
		int numX = this->mSpriteSheet.width() / size;
		int numY = this->mSpriteSheet.height() / size;

		for(int y = 0; y < numY; y++) {
			for(int x = 0; x < numX; x++) {
				QGraphicsPixmapItem * pmap = this->mScene->addPixmap(
					QPixmap::fromImage(
						this->mSpriteSheet.copy(size * x, size * y, size, size)));
				pmap->setPos(
					1 + (size + 1) * x,
					1 + (size + 1) * y);
			}
		}

		for(int x = 0; x <= numX; x++) {
			this->mScene->addLine(
				(size + 1) * x,
				0,
				(size + 1) * x,
				(size + 1) * numY,
				QPen(QColor(255,0,2550)));
		}

		for(int y = 0; y <= numY; y++) {
			this->mScene->addLine(
				0,
				(size + 1) * y,
				(size + 1) * numX,
				(size + 1) * y,
				QPen(QColor(255,0,0)));
		}
	}
	else
	{
		this->mScene->setSceneRect(QRect(0,0,0,0));
	}

	this->ui->preview->resetCachedContent();
	this->ui->preview->updateSceneRect(this->mScene->sceneRect());
}

void OpenSpritesheetDialog::on_fileName_textChanged(const QString &fileName)
{
    this->mSourceImage = QImage(fileName).convertToFormat(QImage::Format_ARGB32);
	if(this->mSourceImage.isNull() == false)
	{
		auto limit = qMin(
			this->mSourceImage.width(),
			this->mSourceImage.height());
		this->ui->spriteSize->setMaximum(limit);
		this->ui->spritePadding->setMaximum(limit / 2);
		this->ui->spritesheetMargin->setMaximum(limit / 2);

		this->renderImage();
	}

	this->updatePreview();
	this->checkOk();
}

void OpenSpritesheetDialog::on_spritesheetMargin_valueChanged(int arg1)
{
	Q_UNUSED(arg1);
	this->renderImage();
	this->updatePreview();
	this->checkOk();
}

void OpenSpritesheetDialog::on_spritePadding_valueChanged(int arg1)
{
	Q_UNUSED(arg1);
	this->renderImage();
	this->updatePreview();
	this->checkOk();
}

void OpenSpritesheetDialog::on_spriteSize_valueChanged(int arg1)
{
	Q_UNUSED(arg1);
	this->renderImage();
	this->updatePreview();
	this->checkOk();
}
