#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDockWidget>
#include <QScrollArea>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget(this->mve = new ModelEditorView());

    this->ui->dockWidgetTileset->setWidget(this->tse = new TileSetViewer());
}

MainWindow::~MainWindow()
{
    delete ui;
}
