#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDockWidget>
#include <QScrollArea>
#include <QApplication>
#include <QFileDialog>
#include <QFile>
#include <QDebug>
#include <QMessageBox>
#include <QGroupBox>
#include <QLayout>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDesktopServices>
#include <QRgb>
#include <QKeyEvent>

#include "createmodeldialog.h"
#include "optionsdialog.h"

#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	this->setCentralWidget(this->mve = new ModelEditorView());

	this->tse = new TileSetViewer();

	this->ui->dockWidgetContents->layout()->addWidget(this->tse);
	this->ui->dockWidgetContents->layout()->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

	connect(
	    this->tse, &TileSetViewer::spriteSelected,
	    this->mve, &ModelEditorView::beginInsertSprite);
	connect(
	    this->mve, &ModelEditorView::meshChanged,
	    this, &MainWindow::on_mve_modelHasChanged);

	connect(
		this->mve, &ModelEditorView::selectionCleared,
		this->tse, &TileSetViewer::resetSelection);

	Mesh mesh;
	mesh.texture = QImage(":/data/tilesets/croco.png");
	mesh.minimumTileSize = 16;
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
	if(this->mCurrentFile.suffix() != "v3m") {
		this->mCurrentFile.setFile(this->mCurrentFile.filePath() + ".v3m");
	}
	this->save();
}

void MainWindow::on_actionNew_triggered()
{
	if(this->ensureModelIsSave() == false) {
		return;
	}

	CreateModelDialog dialog(this);
	if(dialog.exec() != QDialog::Accepted) {
		return;
	}

	Mesh mesh;
	mesh.texture = dialog.getTileSet();
	mesh.minimumTileSize = dialog.getMinimumTileSize();
	this->setModel(mesh);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if(this->mModelIsDirty == false) {
		return;
	}
	int result = QMessageBox::question(
		this,
		this->windowTitle(),
	            "Do you want to save your changes?",
		QMessageBox::StandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel),
		QMessageBox::Cancel);
	switch(result)
	{
		case QMessageBox::Yes:
			this->on_actionSave_triggered();
			if(this->mModelIsDirty) {
				event->ignore();
			}
			break;
		case QMessageBox::No:
			return;
		case QMessageBox::Cancel:
			event->ignore();
			break;
	}
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
	this->tse->setMesh(mesh);
	this->mModelIsDirty = false;
}

void MainWindow::on_actionRotateRight_triggered()
{
	this->mve->rotateRight();
}

void MainWindow::on_actionRotateLeft_triggered()
{
	this->mve->rotateLeft();
}

void MainWindow::on_actionFlip_Horizontal_triggered()
{
	this->mve->flipHorizontal();
}

void MainWindow::on_actionFlip_Vertical_triggered()
{
	this->mve->flipVertical();
}

void MainWindow::on_actionDelete_Face_triggered()
{
	this->mve->deleteSelection();
}

void MainWindow::on_toolButtonZoomX1_clicked()
{
	this->tse->setScale(1);
}

void MainWindow::on_toolButtonZoomX2_clicked()
{
	this->tse->setScale(2);
}

void MainWindow::on_toolButtonZoomX3_clicked()
{
	this->tse->setScale(3);
}

void MainWindow::on_toolButtonZoomX4_clicked()
{
	this->tse->setScale(4);
}

void MainWindow::on_actionGitHub_Page_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/MasterQ32/VersaTile"));
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(
		this,
		this->windowTitle(),
		"VersaTile is a simple 3D model editor.\n"
		"Made by: Felix ¨MasterQ32¨ Queißner\n"
		"Licence: GNU Puplic Licence");
}

static aiVector3D C(const Mesh & mesh, const glm::ivec3 & vec)
{
	Q_UNUSED(mesh);
	return aiVector3D(vec.x, vec.y, vec.z);
}

static aiVector3D C(const Mesh &mesh, const glm::ivec2 &vec)
{
	return aiVector3D(
		float(vec.x) / float(mesh.texture.width() - 1),
		float(vec.y) / float(mesh.texture.height() - 1),
		0.0f);
}


void MainWindow::on_actionExport_triggered()
{
	using namespace Assimp;

	QStringList filterNames;

	Exporter exporter;
	size_t cnt = exporter.GetExportFormatCount();
	for(size_t i = 0; i < cnt; i++) {
		aiExportFormatDesc const * desc = exporter.GetExportFormatDescription(i);
		filterNames << QString(desc->description) + "(*." + QString(desc->fileExtension) + ")";
	}

	if(this->mCurrentExport.isFile() == false) {
		this->mCurrentExport = QFileInfo(this->mCurrentFile);
	}

	QFileDialog dialog(this);
	dialog.setNameFilters(filterNames);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setDirectory(this->mCurrentExport.dir());
	if(dialog.exec() != QFileDialog::Accepted) {
		return;
	}

	this->mCurrentExportFilter = dialog.selectedNameFilter();
	int index = filterNames.indexOf(dialog.selectedNameFilter());

	const Mesh & src = this->mve->mesh();

	aiMesh * mesh = new aiMesh;
	mesh->mNumVertices = 4 * src.faces.size();
	mesh->mPrimitiveTypes = aiPrimitiveType_TRIANGLE;
	mesh->mVertices = new aiVector3D[4 * src.faces.size()];
	mesh->mTextureCoords[0] = new aiVector3D[4 * src.faces.size()];
	mesh->mNumUVComponents[0] = 2;
	mesh->mFaces = new aiFace[src.faces.size()];
	mesh->mNumFaces = src.faces.size();
	mesh->mMaterialIndex = 0;

	for(unsigned int i = 0; i < src.faces.size(); i++)
	{
		const Face & face = src.faces[i];

		mesh->mFaces[i].mNumIndices = 4;
		mesh->mFaces[i].mIndices = new unsigned int[4];

		mesh->mFaces[i].mIndices[0] = 4 * i + 0;
		mesh->mFaces[i].mIndices[1] = 4 * i + 1;
		mesh->mFaces[i].mIndices[2] = 4 * i + 3;
		mesh->mFaces[i].mIndices[3] = 4 * i + 2;

		for(unsigned int o = 0; o < 4; o++) {
			mesh->mVertices[4 * i + o] = C(src, face.vertices[o].position);
			mesh->mTextureCoords[0][4 * i + o] = C(src, face.vertices[o].uv);
		}
	}

	unsigned int meshIndex = 0;

	aiNode * node = new aiNode;
	node->mMeshes = &meshIndex;
	node->mNumMeshes = 1;
	node->mTransformation = aiMatrix4x4();
	node->mName = aiString("root");

	int texIndex = 0;
	aiString name("TilesetTexture");
	aiColor3D diffuse(1.0f, 1.0f, 1.0f);

	aiMaterial * material = new aiMaterial;
	material->AddProperty(&name, AI_MATKEY_NAME);
	material->AddProperty(&texIndex, 1, AI_MATKEY_TEXTURE_DIFFUSE(0));
	material->AddProperty(&texIndex, 1, AI_MATKEY_TEXTURE_AMBIENT(0));
	material->AddProperty(&diffuse, 1, AI_MATKEY_COLOR_DIFFUSE);
	material->AddProperty(&diffuse, 1, AI_MATKEY_COLOR_AMBIENT);

	aiMesh ** meshes =  new aiMesh*[1];
	meshes[0] = mesh;

	aiMaterial ** materials = new aiMaterial*[1];
	materials[0] = material;

	aiTexture ** textures = new aiTexture*[1];
	textures[0] = new aiTexture;
	textures[0]->mWidth = src.texture.width();
	textures[0]->mHeight = src.texture.height();
	textures[0]->pcData = new aiTexel[src.texture.width() * src.texture.height()];
	for(unsigned y = 0; y < textures[0]->mHeight; y++) {
		for(unsigned x = 0; x < textures[0]->mWidth; x++) {
			QRgb col = src.texture.pixel(x, y);
			aiTexel & texel = textures[0]->pcData[textures[0]->mWidth * y + x];
			texel.r = uint8_t(qRed(col));
			texel.g = uint8_t(qGreen(col));
			texel.b = uint8_t(qBlue(col));
			texel.a = uint8_t(qAlpha(col));
		}
	}
	memcpy(textures[0]->achFormatHint, "png", 4);

	aiScene * scene = new aiScene;
	scene->mNumMeshes = 1;
	scene->mNumMaterials = 1;
	scene->mNumTextures = 1;
	scene->mMeshes = meshes;
	scene->mRootNode = node;
	scene->mMaterials = materials;
	scene->mTextures = textures;

	aiReturn result = exporter.Export(
		scene,
		exporter.GetExportFormatDescription(index)->id,
		dialog.selectedFiles()[0].toStdString(),
	    aiProcess_Triangulate,
		nullptr);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
	qDebug() << "fancy" << event->key();
}

void MainWindow::on_actionHome_triggered()
{
    this->mve->gotoCameraHome();
}

void MainWindow::on_actionSet_Home_triggered()
{
    this->mve->setCameraHome();
}

void MainWindow::on_actionTop_triggered()
{
    this->mve->gotoCameraTop();
}

void MainWindow::on_actionFront_triggered()
{
	this->mve->gotoCameraFront();
}

void MainWindow::on_actionSide_triggered()
{
	this->mve->gotoCameraSide();
}

void MainWindow::on_actionRedo_triggered()
{
    this->mve->redo();
}

void MainWindow::on_actionPreferences_triggered()
{
	OptionsDialog dialog(this);
	dialog.exec();
	this->mve->loadSettings();
	// this->tse->update();
}

void MainWindow::on_actionSelection_Mode_triggered()
{
    this->mve->resetInsertMode();
}
