#ifndef MODELEDITORVIEW_H
#define MODELEDITORVIEW_H

#include <GL/gl3w.h>
#include <QOpenGLWidget>
#include <QOpenGLTexture>
#include <QScopedPointer>
#include <QStack>

#include "mesh.h"

class ModelEditorView : public QOpenGLWidget
{
	Q_OBJECT
public:
	enum Tool
	{
		Select,
		Move,
		DisplaceVertex,
		Create,
	};
	enum RotateDir
	{
		Left,
		Right
	};
	enum Flipping
	{
		None = 0,
		Horizontal = 1,
		Vertical = 2,
	};
public:
	explicit ModelEditorView(QWidget *parent = 0);

	virtual void initializeGL() override;

	virtual void resizeGL(int w, int h) override;

	virtual void paintGL() override;

	void setMesh(const Mesh & mesh);

	const Mesh & mesh() const { return this->mMesh; }

	Mesh & mesh() { return this->mMesh; }

	bool hasMesh() const
	{
		return !this->mMesh.texture.isNull();
	}

	void setPlaneAxis(int axis)
	{
		this->mPlaneAxis = axis % 3;
		this->repaint();
	}

	glm::ivec3 planeNormal() const
	{
		glm::ivec3 normal(0,0,0);
		normal[this->mPlaneAxis % 3] = 1;
		return normal;
	}

	void switchToTool(Tool tool);

	void beginInsertSprite(QRect rect) {
		this->mSpriteToInsert = rect;
		this->mSpriteToInsertRotation = 0;
		this->mSpriteToInsertFlipping = None;
		this->mCurrentTool = Create;
		this->repaint();
	}

	void loadSettings();

	void undo();

	void redo();

	void rotateRight();

	void rotateLeft();

	void flipVertical();

	void flipHorizontal();

	void deleteSelection();

	void clearSelection() {
		this->mSelectedFace = -1;
		this->updateGizmos();
		this->repaint();
	}

public:

	virtual void focusInEvent(QFocusEvent *event) override;

	virtual void focusOutEvent(QFocusEvent *event) override;

	virtual void mouseMoveEvent(QMouseEvent *event) override;

	virtual void mousePressEvent(QMouseEvent *event) override;

	virtual void mouseReleaseEvent(QMouseEvent *event) override;

	virtual void wheelEvent(QWheelEvent *event) override;

	virtual void keyPressEvent(QKeyEvent *event) override;
	virtual void keyReleaseEvent(QKeyEvent *event) override;
private:
	void getPlane(glm::ivec3 & normal,glm::ivec3 & tangent, glm::ivec3 & cotangent) const;

	void getPlane(int index, glm::ivec3 & normal,glm::ivec3 & tangent, glm::ivec3 & cotangent) const;

	void getRay(int x, int y, glm::vec3 & origin, glm::vec3 & direction) const;

	int determinePlane(const glm::vec3 & direction);

	glm::ivec3 raycastAgainstPlane(int x, int y) const;

	glm::ivec3 raycastAgainstPlane(glm::vec3 origin, glm::vec3 normal, int x, int y) const;

	bool getFaceToInsert(Face & face);

	void addUndoStep();

	void rotateFace(Face & face, RotateDir dir);

	/**
	 * @brief Gets a pointer to the current selection if any.
	 * @return pointer to the currently selected face or nullptr.
	 */
	Face * getSelection() {
		if(this->hasSelection()) {
			return &this->mMesh.faces[this->mSelectedFace];
		} else {
			return nullptr;
		}
	}

	bool hasSelection() {
		if(this->mCurrentTool == Create) {
			return false;
		}
		return this->mSelectedFace >= 0 && this->mSelectedFace < int(this->mMesh.faces.size());
	}

	bool hasInsertion() {
		return (this->mCurrentTool == Create);
	}

	void updateGizmos();

signals:

	void meshIsAboutToChange();
	void meshChanged();
	void selectionCleared();

private:
	void setPan(const QVariant & value);
	void setTilt(const QVariant & value);
	void setZoom(const QVariant & value);
	void limitTilt();
	void limitZoom();

	void animate(float from, float to, void (ModelEditorView::*target)(const QVariant &));

public:
	void setCameraHome();

	void gotoCameraHome();
	void gotoCameraFront();
	void gotoCameraTop();
	void gotoCameraSide();

private:
	void updateAutoGrid();
	void selectNextGrid();

private:
	Mesh mMesh;
	QScopedPointer<QOpenGLTexture> mTexture;
	QScopedPointer<QOpenGLTexture> mPixel;
	GLuint shader, vao, vbuffer;
	bool mOpenGLReady;
	GLint locMatTransform, locTexDiffuse, locVecTint, locIAlphaTest;
	QPoint mLastMouse;
	float mPan, mTilt, mZoom;
	glm::vec3 mCameraPosition;
	glm::ivec3 mCameraFocus;
	glm::mat4 matViewProj;
	int mPlaneAxis;
	QRect mSpriteToInsert;
	int mSpriteToInsertRotation;
	Flipping mSpriteToInsertFlipping;
	glm::ivec3 mCursorPosition;
	bool mSnapToCoarseGrid;
	QStack<Mesh> mUndoStack, mRedoStack;
	int mSelectedFace;
	Tool mCurrentTool;
	QPoint mGizmoPositions[5]; // stores screen positions of vertex [0]-[3] and sprite center in [4]
	glm::ivec3 mMoveOffsetToCursor;
	int mMoveVertexIndex;
	glm::ivec3 mMoveVertexOrigin, mMoveVertexDirection;
	glm::vec3 mMoveVertexPlaneNormal; // <- must be vec3 as it can also be smootly rotated

	float mHomePan, mHomeTilt, mHomeZoom;

	bool mAutoGrid;
	float mAutoGridThreshold;
	int mGroundMode, mGroundSize;
};

#endif // MODELEDITORVIEW_H
