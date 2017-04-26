#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDockWidget>
#include <QScrollArea>
#include <QApplication>
#include <QFileDialog>
#include <QFile>
#include <QDebug>
#include <QMessageBox>

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
    connect(
        this->mve, &ModelEditorView::meshChanged,
        this, &MainWindow::on_mve_modelHasChanged);

    Mesh mesh;
    mesh.texture = QImage(":/data/tilesets/croco.png");
    this->setModel(mesh);
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

void MainWindow::on_actionSave_triggered()
{
    if(this->mCurrentFile.fileName().isEmpty()) {
        this->on_actionSave_As_triggered();
    } else {
        this->save();
    }
}

void MainWindow::on_actionSave_As_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "",
        this->mCurrentFile.absolutePath(),
        "Versa-Tile Models (*.v3m)");
    if(fileName.isNull()) {
        return;
    }
    this->mCurrentFile.setFile(fileName);
    this->save();
}

void MainWindow::on_actionNew_triggered()
{
    if(this->ensureModelIsSave() == false) {
        return;
    }
    Mesh mesh;
    mesh.texture = QImage(":/data/tilesets/croco.png");
    this->setModel(mesh);
}

void MainWindow::on_actionOpen_triggered()
{
    if(this->ensureModelIsSave() == false) {
        return;
    }
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "",
        this->mCurrentFile.absolutePath(),
        "Versa-Tile Models (*.v3m)");
    if(fileName.isNull()) {
        return;
    }

    QFile file(fileName);
    if(file.open(QFile::ReadOnly) == false) {
        QMessageBox::warning(
            this,
            this->windowTitle(),
            tr("Failed to open file!"));
        return;
    }

    Mesh mesh;
    mesh.load(file);
    file.close();

    this->setModel(mesh);
    this->mCurrentFile.setFile(fileName);

}

void MainWindow::on_mve_modelHasChanged()
{
    if(this->mModelIsDirty == false) {
        qDebug() << "Mark model as dirty...";
    }
    this->mModelIsDirty = true;
}

bool MainWindow::ensureModelIsSave()
{
    if(this->mModelIsDirty == false) {
        return true;
    }
    int result = QMessageBox::question(
        this,
        this->windowTitle(),
        tr("Do you want to save your changes?"),
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
        QMessageBox::NoButton);
    switch(result)
    {
        case QMessageBox::Yes:
            this->on_actionSave_triggered();
            return (this->mModelIsDirty == false);
        case QMessageBox::No:
            return true;
        case QMessageBox::Cancel:
            return false;
        default:
            qWarning() << "Unhandled msg box option: " << result;
            return false;
    }
}

void MainWindow::save()
{
    qDebug() << "Save model to" << this->mCurrentFile.absoluteFilePath();
    QFile file(this->mCurrentFile.absoluteFilePath());
    if(file.open(QFile::WriteOnly) == false) {
        QMessageBox::critical(
            this,
            this->windowTitle(),
            tr("Failed to save file!"));
        return;
    }
    this->mve->mesh().save(file);
    file.flush();
    file.close();

    this->mModelIsDirty = false;
}

void MainWindow::setModel(const Mesh & mesh)
{
    qDebug() << "Change mesh, set mesh as clean.";
    this->mve->setMesh(mesh);
    this->tse->setTexture(mesh.texture);
    this->mModelIsDirty = false;
}
