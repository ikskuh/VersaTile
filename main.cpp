#include "mainwindow.h"
#include <QApplication>
int main(int argc, char *argv[])
{
	QSurfaceFormat fmt;
    fmt.setVersion(3, 3);
    QSurfaceFormat::setDefaultFormat(fmt);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
