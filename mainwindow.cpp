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


    Mesh mesh;
    mesh.texture = QImage(":/data/tilesets/croco.png");
    mesh.faces.push_back(Face
    {
        {
            Vertex(glm::ivec3( 0,  0,  0), glm::ivec2(208+0, 80+ 0)),
            Vertex(glm::ivec3(32,  0,  0), glm::ivec2(208+32,80+ 0)),
            Vertex(glm::ivec3( 0, 32,  0), glm::ivec2(208+0, 80+32)),
            Vertex(glm::ivec3(32, 32,  0), glm::ivec2(208+32,80+32)),
        }
    });

    this->mve->setMesh(mesh);

    this->tse->setTexture(mesh.texture);
}

MainWindow::~MainWindow()
{
    delete ui;
}
