#ifndef TILESETVIEWER_H
#define TILESETVIEWER_H

#include <QWidget>
#include <QImage>

class TileSetViewer : public QWidget
{
    Q_OBJECT
public:
    explicit TileSetViewer(QWidget *parent = 0);

    virtual QSize sizeHint() const override;

    virtual QSize minimumSizeHint() const override;

    virtual void paintEvent(QPaintEvent *event) override;

    virtual void mouseMoveEvent(QMouseEvent *event) override;

    virtual void mousePressEvent(QMouseEvent *event) override;

    virtual void mouseReleaseEvent(QMouseEvent *event) override;

    void setTexture(const QImage & tex);

    const QImage & texture() const {
        return this->mTexture;
    }

signals:
    void spriteSelected(QRect sprite);

public slots:

private:
    QImage mTexture;
    QRect mSelection;
    QRect mStartSelection;
};

#endif // TILESETVIEWER_H
