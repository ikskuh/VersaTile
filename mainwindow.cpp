#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDockWidget>
#include <QScrollArea>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget(this->mve = new ModelEditorView());

    this->ui->dockWidgetTileset->setWidget(this->tse = new TileSetViewer());

    connect(
        this->tse, &TileSetViewer::spriteSelected,
        this->mve, &ModelEditorView::beginInsertSprite);

    Mesh mesh;
    mesh.texture = QImage(":/data/tilesets/croco.png");

    this->mve->setMesh(mesh);

    this->tse->setTexture(mesh.texture);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionUndo_triggered()
{
    this->mve->undo();
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::quit();
}
