#include "tilesetviewer.h"

#include <QPaintEvent>
#include <QPainter>

TileSetViewer::TileSetViewer(QWidget *parent) : QWidget(parent)
{

}

QSize TileSetViewer::sizeHint() const
{
    return QSize(256, 256);
}

QSize TileSetViewer::minimumSizeHint() const
{
    return QSize(256, 256);
}

void TileSetViewer::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawLine(
        0, 0,
        this->size().width(),
        this->size().height());

    event->accept();
}
