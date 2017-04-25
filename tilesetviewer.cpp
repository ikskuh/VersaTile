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
    this->repaint();
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
            this->mTexture.height() + 8);
    }
}

void TileSetViewer::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    if(this->mTexture.isNull())
        return;

    painter.setPen(QColor(0, 0, 0));
    painter.drawRect(
        1, 1,
        this->mTexture.width() + 1,
        this->mTexture.width() + 1);
    painter.drawImage(
        2, 2,
        this->mTexture);

    if(this->mSelection.width() > 0 && this->mSelection.height() > 0)
    {
        painter.setPen(QColor(255, 255, 255));
        painter.drawRect(
            16 * this->mSelection.x() + 1,
            16 * this->mSelection.y() + 1,
            16 * this->mSelection.width() + 1,
            16 * this->mSelection.height() + 1);
    }

    event->accept();
}

void TileSetViewer::mouseMoveEvent(QMouseEvent *event)
{
    int x = event->x() - 4;
    int y = event->y() - 4;
    if(x < 0 || x >= this->mTexture.width()) {
        return;
    }
    if(y < 0 || y >= this->mTexture.height()) {
        return;
    }

    int tx = x / 16;
    int ty = y / 16;

    if(event->buttons() & Qt::LeftButton) {
        this->mSelection = QRect(tx, ty, 1, 1).united(this->mStartSelection);
    }
    this->repaint();
}

void TileSetViewer::mousePressEvent(QMouseEvent *event)
{
    int x = event->x() - 4;
    int y = event->y() - 4;
    if(x < 0 || x >= this->mTexture.width()) {
        return;
    }
    if(y < 0 || y >= this->mTexture.height()) {
        return;
    }

    int tx = x / 16;
    int ty = y / 16;

    if(event->button() == Qt::LeftButton) {
        this->mStartSelection = QRect(tx, ty, 1, 1);
        this->mSelection = this->mStartSelection;
    }
    this->repaint();
}

void TileSetViewer::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        if(this->mSelection.width() > 0) {
            // finally, we have selected something :)
        }
    }
}
