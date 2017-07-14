#ifndef FILESELECTIONEDIT_H
#define FILESELECTIONEDIT_H

#include <QLineEdit>
#include <QToolButton>

class FileSelectionEdit : public QLineEdit
{
	Q_OBJECT
public:
	explicit FileSelectionEdit(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void buttonClicked();

private:
    QToolButton *searchButton;
};

#endif // FILESELECTIONEDIT_H
