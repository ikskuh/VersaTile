#include "tilesetviewer.h"

#include <QPaintEvent>
#include <QPainter>
#include <QDebug>

TileSetViewer::TileSetViewer(QWidget *parent) : QWidget(parent), mScale(1)
{

}

void TileSetViewer::setMesh(const Mesh & mesh)
{
	this->mMesh = mesh;
	if(this->texture().isNull() == false) {
		this->setMaximumSize(this->minimumSizeHint());
	}
	this->repaint();
}

QSize TileSetViewer::sizeHint() const
{
	return this->minimumSizeHint();
}

QSize TileSetViewer::minimumSizeHint() const
{
	if(this->texture().isNull()) {
		return QSize(256, 256);
	} else {
		return QSize(
		    this->mScale * this->texture().width() + 3,
		    this->mScale * this->texture().height() + 3);
	}
}

void TileSetViewer::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);

	if(this->texture().isNull())
		return;

	painter.setPen(QColor(0, 0, 0));
	painter.drawRect(
	    1, 1,
	    this->mScale * this->texture().width() + 1,
	    this->mScale * this->texture().width() + 1);
	painter.drawImage(
	    QRect(2, 2, this->texture().width() * this->mScale, this->texture().height() * this->mScale),
	    this->texture());

	if(this->mSelection.width() > 0 && this->mSelection.height() > 0)
	{
		painter.setPen(QColor(255, 255, 255));
		painter.drawRect(
		    this->mScale * this->tileSize() * this->mSelection.x() + 1,
		    this->mScale * this->tileSize() * this->mSelection.y() + 1,
		    this->mScale * this->tileSize() * this->mSelection.width() + 1,
		    this->mScale * this->tileSize() * this->mSelection.height() + 1);
	}

	event->accept();
}

void TileSetViewer::mouseMoveEvent(QMouseEvent *event)
{
	int x = event->x() - 4;
	int y = event->y() - 4;
	if(x < 0 || x >= (this->mScale * this->texture().width())) {
		return;
	}
	if(y < 0 || y >= (this->mScale * this->texture().height())) {
		return;
	}

	int tx = (x / this->mScale) / this->tileSize();
	int ty = (y / this->mScale) / this->tileSize();

	if(event->buttons() & Qt::LeftButton) {
		this->mSelection = QRect(tx, ty, 1, 1).united(this->mStartSelection);
	}
	this->repaint();
}

void TileSetViewer::mousePressEvent(QMouseEvent *event)
{
	int x = event->x() - 4;
	int y = event->y() - 4;
	if(x < 0 || x >= this->texture().width()) {
		return;
	}
	if(y < 0 || y >= this->texture().height()) {
		return;
	}

	int tx = (x / this->mScale) / this->tileSize();
	int ty = (y / this->mScale) / this->tileSize();

	if(event->button() == Qt::LeftButton) {
		this->mStartSelection = QRect(tx, ty, 1, 1);
		this->mSelection = this->mStartSelection;
		qDebug() << this->mStartSelection << this->mSelection;
	}
	this->repaint();
}

void TileSetViewer::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		if(this->mSelection.width() > 0) {
			// finally, we have selected something :)
			this->spriteSelected(QRect(
				this->tileSize() * this->mSelection.x(),
				this->tileSize() * this->mSelection.y(),
				this->tileSize() * this->mSelection.width(),
				this->tileSize() * this->mSelection.height()));
		}
	}
}
