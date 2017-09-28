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

#include "optionsdialog.h"
#include "openspritesheetdialog.h"

#include <assimp/IOSystem.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	this->mve = this->ui->modelViewEditor;
	this->tse = this->ui->tileSetViewer;

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
	mesh.texture = OpenSpritesheetDialog::renderImage(
		QImage(":/data/tilesets/roguelike-caves.png"),
		16,
		1,
		0);
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
	OpenSpritesheetDialog dialog(this);
	dialog.setSpriteSize(this->mve->mesh().minimumTileSize);
	if(dialog.exec() != QDialog::Accepted) {
		return;
	}

	Mesh mesh;
	mesh.texture = dialog.spriteSheet();
	mesh.minimumTileSize = dialog.spriteSize();
	this->setModel(mesh);
    this->mModelIsDirty = false;
    updateTitle();
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
    updateTitle();
}

void MainWindow::on_mve_modelHasChanged()
{
	if(this->mModelIsDirty == false) {
		qDebug() << "Mark model as dirty...";
	}
	this->mModelIsDirty = true;
    updateTitle();
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
    updateTitle();
}

void MainWindow::setModel(const Mesh & mesh)
{
	qDebug() << "Change mesh, set mesh as clean.";
	this->mve->setMesh(mesh);
	this->tse->setMesh(mesh);
	this->mModelIsDirty = false;
    updateTitle();
}

void MainWindow::updateTitle()
{
    QString title;

    if(mCurrentFile.exists()) {
        title += mCurrentFile.fileName();
        if(mModelIsDirty == true) {
            title += "* - ";
        }
        else {
            title += " - ";
        }
    }

    title += "VersaTile 3D Editor";
    this->setWindowTitle(title);
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

static aiVector3D C(const Mesh & mesh, const glm::vec3 & vec)
{
	Q_UNUSED(mesh);
	return aiVector3D(vec.x, vec.y, vec.z);
}

static aiVector3D C(const Mesh &mesh, const glm::ivec2 &vec)
{
	return aiVector3D(
		float(vec.x) / float(mesh.texture.width()),
		float(vec.y) / float(mesh.texture.height()),
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

	QFileInfo fileInfo(dialog.selectedFiles()[0]);

	QString textureName = fileInfo.absolutePath()
	        + "/"
	        + fileInfo.completeBaseName()
	        + ".png";

	this->mCurrentExportFilter = dialog.selectedNameFilter();
	int index = filterNames.indexOf(dialog.selectedNameFilter());

	const Mesh & src = this->mve->mesh();

	std::vector<aiVector3D> positions;
	std::vector<aiVector3D> normals;
	std::vector<aiVector3D> uvcoords;

	auto emitVertex = [&](Vertex const & vtx, glm::vec3 const & normal) -> size_t
	{
		size_t idx = positions.size();
		positions.push_back(C(src, vtx.position));
		normals.push_back(C(src, normal));
		uvcoords.push_back(C(src, vtx.uv));
		return idx;
	};

	aiMesh * mesh = new aiMesh;
	mesh->mPrimitiveTypes = aiPrimitiveType_TRIANGLE;
	mesh->mFaces = new aiFace[2 * src.faces.size()];
	mesh->mNumFaces = 2 * src.faces.size();
	mesh->mMaterialIndex = 0;

	for(unsigned int i = 0; i < src.faces.size(); i++)
	{
		const Face & face = src.faces[i];

		aiFace & dst0 = mesh->mFaces[2*i+0];
		aiFace & dst1 = mesh->mFaces[2*i+1];

		dst0.mNumIndices = 3;
		dst0.mIndices = new unsigned int[3];

		dst1.mNumIndices = 3;
		dst1.mIndices = new unsigned int[3];

		glm::vec3 n0 = glm::normalize(glm::cross(
			glm::vec3(face.vertices[1].position - face.vertices[0].position),
			glm::vec3(face.vertices[3].position - face.vertices[0].position)));
		dst0.mIndices[0] = emitVertex(face.vertices[0], n0);
		dst0.mIndices[1] = emitVertex(face.vertices[1], n0);
		dst0.mIndices[2] = emitVertex(face.vertices[3], n0);

		glm::vec3 n1 = glm::normalize(glm::cross(
			glm::vec3(face.vertices[2].position - face.vertices[0].position),
			glm::vec3(face.vertices[3].position - face.vertices[0].position)));
		dst1.mIndices[0] = emitVertex(face.vertices[0], n1);
		dst1.mIndices[1] = emitVertex(face.vertices[2], n1);
		dst1.mIndices[2] = emitVertex(face.vertices[3], n1);
	}

	mesh->mNumVertices = positions.size();
	mesh->mVertices = new aiVector3D[mesh->mNumVertices];
	mesh->mNormals = new aiVector3D[mesh->mNumVertices];
	mesh->mTextureCoords[0] = new aiVector3D[mesh->mNumVertices];
	mesh->mNumUVComponents[0] = 2;

	memcpy(mesh->mVertices, positions.data(), sizeof(aiVector3D) * mesh->mNumVertices);
	memcpy(mesh->mNormals, normals.data(), sizeof(aiVector3D) * mesh->mNumVertices);
	memcpy(mesh->mTextureCoords[0], uvcoords.data(), sizeof(aiVector3D) * mesh->mNumVertices);

	unsigned int meshIndex = 0;

	aiNode * node = new aiNode;
	node->mMeshes = &meshIndex;
	node->mNumMeshes = 1;
	node->mTransformation = aiMatrix4x4();
	node->mName = aiString("root");

	aiString name("TilesetTexture");
	aiColor3D diffuse(1.0f, 1.0f, 1.0f);
	aiString texName(textureName.toUtf8().data());
	int btrue = 1;

	aiMaterial * material = new aiMaterial;
	material->AddProperty(&name, AI_MATKEY_NAME);
	material->AddProperty(&btrue, 1, AI_MATKEY_TWOSIDED);
	material->AddProperty(&texName, AI_MATKEY_TEXTURE_DIFFUSE(0));
	material->AddProperty(&texName, AI_MATKEY_TEXTURE_AMBIENT(0));
	material->AddProperty(&diffuse, 1, AI_MATKEY_COLOR_DIFFUSE);

	aiMesh ** meshes =  new aiMesh*[1];
	meshes[0] = mesh;

	aiMaterial ** materials = new aiMaterial*[1];
	materials[0] = material;

	aiScene * scene = new aiScene;
	scene->mNumMeshes = 1;
	scene->mNumMaterials = 1;
	scene->mMeshes = meshes;
	scene->mRootNode = node;
	scene->mMaterials = materials;

	aiReturn result = exporter.Export(
		scene,
		exporter.GetExportFormatDescription(index)->id,
		fileInfo.absoluteFilePath().toStdString().c_str(),
	    aiProcess_Triangulate | aiProcess_FlipUVs,
		nullptr);

	if(result == aiReturn_SUCCESS)
	{
		src.texture.save(
			textureName,
			"png");
	}
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
}

void MainWindow::on_actionSelection_Mode_triggered()
{
    this->mve->resetInsertMode();
}

void MainWindow::on_actionFocus_Selection_triggered()
{
    this->mve->setCameraToSelection();
}

void MainWindow::on_actionUpdate_texture_triggered()
{
	int currentSize = this->mve->mesh().minimumTileSize;
	OpenSpritesheetDialog dialog(this);
	dialog.setSpriteSize(currentSize);

	bool accepted = false;
	do {

		if(dialog.exec() != QDialog::Accepted) {
			return;
		}

		if(currentSize != dialog.spriteSize()) {
			auto result = QMessageBox::question(
				this,
				this->windowTitle(),
				tr("The current model has a sprite size of %1, but the imported spritesheet has %2.\n"
			       "Do you really want to import the sprite sheet?").arg(currentSize).arg(dialog.spriteSize()),
				QMessageBox::StandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel),
				QMessageBox::Yes);
			switch(result) {
				case QMessageBox::Yes:
					accepted = true;
					break;
				case QMessageBox::No: continue;
				case QMessageBox::Cancel: return;
				default: continue;
			}
		} else {
			accepted = true;
		}

	} while(!accepted);

	QImage texture = dialog.spriteSheet();
	int spriteSize = dialog.spriteSize();

	Mesh mesh(this->mve->mesh());
	mesh.texture = texture;
	mesh.minimumTileSize = spriteSize;
	this->mve->setMesh(mesh);
	this->tse->setMesh(mesh);
}

void MainWindow::on_actionSwap_Grid_Plane_triggered()
{
    this->mve->selectNextGrid();
}

void MainWindow::on_actionHomepage_triggered()
{
    QDesktopServices::openUrl(QUrl("https://mq32.de/projects/versatile.htm"));
}
