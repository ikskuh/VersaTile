#include "tilesetviewer.h"

#include <QPaintEvent>
#include <QPainter>

TileSetViewer::TileSetViewer(QWidget *parent) : QWidget(parent)
{

}

void TileSetViewer::setTexture(const QImage & tex)
{
    this->mTexture = tex;
    if(this->mTexture.isNull() == false) {
        this->setMaximumSize(this->minimumSizeHint());
    }
    this->update();
}

QSize TileSetViewer::sizeHint() const
{
    return this->minimumSizeHint();
}

QSize TileSetViewer::minimumSizeHint() const
{
    if(this->mTexture.isNull()) {
        return QSize(256, 256);
    } else {
        return QSize(
            this->mTexture.width() + 8,
            this->mTexture.height() + 4);
    }
}

void TileSetViewer::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    if(this->mTexture.isNull())
        return;

    painter.drawRect(
        1, 1,
        this->mTexture.width() + 1,
        this->mTexture.width() + 1);
    painter.drawImage(
        2, 2,
        this->mTexture);

    event->accept();
}

void TileSetViewer::mouseMoveEvent(QMouseEvent *event)
{

}

void TileSetViewer::mousePressEvent(QMouseEvent *event)
{

}

void TileSetViewer::mouseReleaseEvent(QMouseEvent *event)
{

}
