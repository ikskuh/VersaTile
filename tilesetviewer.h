#ifndef TILESETVIEWER_H
#define TILESETVIEWER_H

#include <QWidget>

class TileSetViewer : public QWidget
{
    Q_OBJECT
public:
    explicit TileSetViewer(QWidget *parent = 0);

    virtual QSize sizeHint() const override;

    virtual QSize minimumSizeHint() const override;

    virtual void paintEvent(QPaintEvent *event) override;
signals:

public slots:
};

#endif // TILESETVIEWER_H
