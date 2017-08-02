#ifndef TILESETVIEWER_H
#define TILESETVIEWER_H

#include <QWidget>
#include <QImage>
#include "mesh.h"

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

	void resetSelection() {
		this->mSelection = QRect();
		this->repaint();
	}

	void setMesh(const Mesh & mesh);

	const Mesh & mesh() const {
		return this->mMesh;
	}

	const QImage & texture() const {
		return this->mMesh.texture;
	}

	int tileSize() const {
		return this->mMesh.minimumTileSize;
	}

	int scale() const { return this->mScale; }

	void setScale(int scale)
	{
		this->mScale = scale;
		if(this->mScale < 0) this->mScale = 1;
		if(this->mScale > 4) this->mScale = 4;
		this->setFixedSize(this->minimumSizeHint());
		this->updateGeometry();
		this->repaint();
	}

signals:
	void spriteSelected(QRect sprite);

public slots:

private:
	Mesh mMesh;
	QRect mSelection;
	QRect mStartSelection;
	int mScale;
};

#endif // TILESETVIEWER_H
