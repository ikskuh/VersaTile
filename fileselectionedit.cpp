#include "fileselectionedit.h"

#include <QStyle>
#include <QDebug>
#include <QFileDialog>

FileSelectionEdit::FileSelectionEdit(QWidget *parent) : QLineEdit(parent)
{
	this->searchButton = new QToolButton(this);
	this->searchButton->setText("…");
	this->searchButton->setCursor(Qt::ArrowCursor);
	this->searchButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
	this->searchButton->setToolTip("Click to search file");
	connect(this->searchButton, &QToolButton::clicked, this, &FileSelectionEdit::buttonClicked);
	int frameWidth = this->style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
	setStyleSheet(QString("QLineEdit { padding-right: %1px; } ").arg(this->searchButton->sizeHint().width() + frameWidth + 1));
	QSize msz = minimumSizeHint();
	setMinimumSize(qMax(msz.width(), this->searchButton->sizeHint().height() + frameWidth * 2 + 2),
				   qMax(msz.height(), this->searchButton->sizeHint().height() + frameWidth * 2 + 2));
}


void FileSelectionEdit::resizeEvent(QResizeEvent *)
{
    QSize sz = this->searchButton->sizeHint();
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    this->searchButton->move(rect().right() - frameWidth - sz.width(),
                      (rect().bottom() + 1 - sz.height())/2);
}

void FileSelectionEdit::buttonClicked()
{
	QString file = QFileDialog::getOpenFileName(
		this,
		"Select the tileset you want to use",
		this->text(),
		"Image Files (*.png *.jpg *.bmp)");
	if(file.isNull()) {
		return;
	}
	this->setText(file);
}
