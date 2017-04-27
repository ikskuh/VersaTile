#include "modeleditorview.h"

#include <QDebug>
#include <QFile>
#include <QMouseEvent>
#include <QWheelEvent>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

static void sysOpenGLDebug(GLenum,GLenum,GLuint,GLenum,GLsizei,const GLchar*,const void *);

static QDebug operator<<(QDebug debug, const glm::vec3 &c)
{
	QDebugStateSaver saver(debug);
	debug.nospace() << '(' << c.x << "; " << c.y << "; " << c.z << ')';
	return debug;
}
static QDebug operator<<(QDebug debug, const glm::ivec3 &c)
{
	QDebugStateSaver saver(debug);
	debug.nospace() << '(' << c.x << "; " << c.y << "; " << c.z << ')';
	return debug;
}

static inline glm::ivec2 C(const QPoint &c)
{
	return glm::ivec2(c.x(), c.y());
}

ModelEditorView::ModelEditorView(QWidget *parent) :
    QOpenGLWidget(parent),
    mMesh(),
    mTexture(nullptr),
    mPixel(nullptr),
    shader(0),
    vao(0),
    vbuffer(0),
    mOpenGLReady(false),
    mPan(0.0f), mTilt(0.0f), mZoom(128.0f),
    mCameraPosition(), mCameraFocus(), matViewProj(),
    mPlaneAxis(2),
    mSpriteToInsert(), mSpriteToInsertRotation(0), mSpriteToInsertFlipping(None),
    mSnapToCoarseGrid(true),
    mUndoStack(),
    mSelectedFace(-1),
    mCurrentTool(Select)
{
	QSurfaceFormat fmt;
	fmt.setVersion(3, 3);
	fmt.setSamples(8);
	this->setFormat(fmt);
	this->setMouseTracking(true);
	// this->grabKeyboard();

	connect(
	            this, &ModelEditorView::meshIsAboutToChange,
	            this, &ModelEditorView::addUndoStep);
}

void ModelEditorView::focusInEvent(QFocusEvent *event)
{
	Q_UNUSED(event)
	this->grabKeyboard();
}

void ModelEditorView::focusOutEvent(QFocusEvent *event)
{
	Q_UNUSED(event)
	this->releaseKeyboard();
}

void ModelEditorView::addUndoStep()
{
	// Callback when mesh is about to change
	this->mUndoStack.push(this->mMesh);
}

void ModelEditorView::getPlane(glm::ivec3 & normal,glm::ivec3 & tangent, glm::ivec3 & cotangent) const
{
	this->getPlane(this->mPlaneAxis, normal, tangent, cotangent);
}

int ModelEditorView::determinePlane(const glm::vec3 & direction)
{
	float dot = 0.0f;
	int idx = 0;
	// Adjust current plane
	for(int i = 0; i < 6; i++)
	{
		glm::ivec3 normal, tangent, cotangent;
		this->getPlane(i, normal, tangent, cotangent);
		float d = glm::dot(glm::vec3(normal), direction);
		if(d > dot) {
			idx = i;
			dot = d;
		}
	}
	return idx;
}

void ModelEditorView::getPlane(int index, glm::ivec3 & normal,glm::ivec3 & tangent, glm::ivec3 & cotangent) const
{
	using namespace glm;
	switch(index % 3)
	{
		case 0:
			normal    = ivec3(1, 0, 0);
			tangent   = ivec3(0, 0, -1);
			cotangent = ivec3(0, 1, 0);
			break;
		case 1:
			normal    = ivec3(0, 1, 0);
			tangent   = ivec3(1, 0, 0);
			cotangent = ivec3(0, 0, 1);
			break;
		case 2:
			normal    = ivec3(0, 0, 1);
			tangent   = ivec3(1, 0, 0);
			cotangent = ivec3(0, 1, 0);
			break;
	}
	if(index >= 3) {
		normal = -normal;
		tangent = -tangent;
		cotangent = -cotangent;
		tangent.y = -tangent.y;
		cotangent.y = -cotangent.y;
	}
}

void ModelEditorView::keyPressEvent(QKeyEvent *event)
{
	glm::vec3 cameraDirection(sin(this->mPan), 0.0f, cos(this->mPan));
	int movementPlane = this->determinePlane(cameraDirection);


	glm::ivec3 normal, tangent, cotangent;
	this->getPlane(movementPlane, normal, tangent, cotangent);

	int stepSize = 1;
	if(this->mSnapToCoarseGrid) {
		stepSize = 16;
	}

	event->setAccepted(true);

	switch(event->key())
	{
		case Qt::Key_W:
			this->mCameraFocus += stepSize * normal;
			break;
		case Qt::Key_S:
			this->mCameraFocus -= stepSize * normal;
			break;
		case Qt::Key_A:
			this->mCameraFocus += stepSize * tangent;
			break;
		case Qt::Key_D:
			this->mCameraFocus -= stepSize * tangent;
			break;
		case Qt::Key_E:
			this->mCameraFocus.y += stepSize;
			break;
		case Qt::Key_Q:
			this->mCameraFocus.y -= stepSize;
			break;
		case Qt::Key_Space:
			// Right button resets the tool to selection and unselect all
			this->mCurrentTool = Select;
			this->clearSelection();
			break;
		case Qt::Key_Shift:
			this->mSnapToCoarseGrid = false;
			break;
		default:
			event->setAccepted(false);
			return;
	}

	this->repaint();
}

void ModelEditorView::keyReleaseEvent(QKeyEvent *event)
{
	if(event->key() == Qt::Key_Shift) {
		this->mSnapToCoarseGrid = true;
	}
}

static int floor0(double value)
{
	// if (value > 0.0)
	//     return ceil( value );
	// else
	return floor( value );
}

static glm::ivec3 floor0(glm::vec3 pos)
{
	return glm::ivec3(floor0(pos.x), floor0(pos.y), floor0(pos.z));
}

void ModelEditorView::mouseMoveEvent(QMouseEvent *event)
{
	int dx = event->x() - this->mLastMouse.x();
	int dy = event->y() - this->mLastMouse.y();
	this->mLastMouse = event->pos();

	this->setCursor(Qt::ArrowCursor);
	for(int i = 0; i < 5; i++)
	{
		QPoint gizmo = this->mGizmoPositions[i];
		gizmo -= event->pos();
		if(gizmo.manhattanLength() < 6) {
			if(i < 4) {
				if(abs(this->planeNormal().y) > 0) {
					this->setCursor(Qt::SplitVCursor);
				} else {
					this->setCursor(Qt::SplitHCursor);
				}
			} else {
				this->setCursor(Qt::SizeAllCursor);
			}
		}
	}


	if(event->buttons() & Qt::RightButton)
	{
		this->mPan  -= 0.03f * dx;
		this->mTilt -= 0.03f * dy;
		if(this->mTilt <= (-0.99 * M_PI_2)) {
			this->mTilt = (-0.99 * M_PI_2);
		}
		if(this->mTilt >= (0.99 * M_PI_2)) {
			this->mTilt = (0.99 * M_PI_2);
		}

		glm::vec3 cameraOffset(
		            sin(this->mPan) * cos(this->mTilt),
		            sin(this->mTilt),
		            cos(this->mPan) * cos(this->mTilt));

		cameraOffset.y *= 0.5f;

		this->mPlaneAxis = this->determinePlane(cameraOffset);

		this->updateGizmos();
	}
	else
	{
		switch(this->mCurrentTool)
		{
			case Move:
			{
				Face * face = this->getSelection();
				if(face == nullptr) {
					this->mCurrentTool = Select;
					break;
				}

				glm::ivec3 newCenter = this->raycastAgainstPlane(
				    face->fulcrum, face->normal,
				    event->x(), event->y());

				// Fixed: Fulcrum must snap to center, not newCenter!
				glm::ivec3 newFulcrum(newCenter - this->mMoveOffsetToCursor);
				if(this->mSnapToCoarseGrid)
				{
					newFulcrum = this->mCameraFocus + 16 * floor0(glm::vec3(newFulcrum - this->mCameraFocus) / 16.0f);
				}

				for(int i = 0; i < 4; i++)
				{
					face->vertices[i].position += (newFulcrum - face->fulcrum);
				}
				face->fulcrum = newFulcrum;

				break;
			}
			default:
			{
				this->mCursorPosition = this->raycastAgainstPlane(event->x(), event->y());
				if(this->mSnapToCoarseGrid)
				{
					// Snap alignment to grid alignment, not to global alignment
					this->mCursorPosition = this->mCameraFocus
					    + 16 * floor0(glm::vec3(this->mCursorPosition - this->mCameraFocus) / 16.0f);
				}
				break;
			}
		}
	}
	this->repaint();
}

void ModelEditorView::mousePressEvent(QMouseEvent *event)
{
	this->setFocus();
	mLastMouse = event->pos();

	glm::ivec3 normal, tangent, cotangent;
	this->getPlane(normal, tangent, cotangent);

	if(event->button() == Qt::LeftButton)
	{
		Face * selection = this->getSelection();
		if(selection != nullptr)
		{
			// check if we clicked a gizmo
			for(int i = 0; i < 5; i++)
			{
				QPoint gizmo = this->mGizmoPositions[i];
				gizmo -= event->pos();
				qDebug() << i << gizmo;
				if(gizmo.manhattanLength() < 4) {
					if(i == 4) {

						glm::ivec3 newCenter = this->raycastAgainstPlane(
						    selection->fulcrum, selection->normal,
						    event->x(), event->y());

						this->mMoveOffsetToCursor = newCenter - selection->fulcrum;
						this->mCurrentTool = Move;
						this->meshIsAboutToChange();
						return;
					} else {
						qDebug() << "Clicked gizmo" << i;
						return;
					}
				}
			}
		}


		// then continue with default action
		switch(this->mCurrentTool)
		{
			case Select:
			{
				glm::vec3 origin, direction;
				this->getRay(event->x(), event->y(), origin, direction);

				float minDist = std::numeric_limits<float>::max();
				int index = -1;
				for(int i = 0; i < this->mMesh.faces.size(); i++)
				{
					float dist;
					if(this->mMesh.faces[i].intersects(origin, direction, &dist))
					{
						if(dist < minDist) {
							minDist = dist;
							index = i;
						}
					}
				}

				this->mSelectedFace = index;
				this->updateGizmos();

				break;
			}
			case Create:
			{
				Face face;
				if(this->getFaceToInsert(face))
				{
					this->meshIsAboutToChange();
					this->mMesh.faces.push_back(face);
					this->meshChanged();
				}
				break;
			}
			case Move:
			{
				this->mCurrentTool = Select;
				break;
			}
			default:
			{
				qDebug() << "Tool" << this->mCurrentTool << "is not implemented yet.";
				break;
			}
		}
	}

	this->repaint();
}

void ModelEditorView::mouseReleaseEvent(QMouseEvent *event)
{
	switch(event->button())
	{
		case Qt::LeftButton:
		{
			if(this->mCurrentTool == Select) {
				return;
			}
			if(this->mCurrentTool != Create) {
				this->meshChanged();
				this->mCurrentTool = Select;
			}
			break;
		}
		default:
		{
			break;
		}
	}
}

void ModelEditorView::wheelEvent(QWheelEvent *event)
{
	this->mZoom -= 0.25f * event->delta();
	if(this->mZoom < 32) {
		this->mZoom = 32;
	}
	this->updateGizmos();
	this->repaint();
}

void ModelEditorView::setMesh(const Mesh & mesh)
{
	this->mMesh = mesh;

	if(this->mOpenGLReady)
	{
		this->mTexture.reset(new QOpenGLTexture(this->mMesh.texture, QOpenGLTexture::DontGenerateMipMaps));
		this->mTexture->setWrapMode(QOpenGLTexture::ClampToBorder);
		this->mTexture->setBorderColor(1.0f, 0.0f, 1.0f, 1.0f);
		this->mTexture->setMinificationFilter(QOpenGLTexture::Nearest);
		this->mTexture->setMagnificationFilter(QOpenGLTexture::Nearest);
	}
	this->clearSelection();
	this->repaint();
}

static void sysOpenGLDebug(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar * message,
        const void *userParam)
{
	Q_UNUSED(source);
	Q_UNUSED(type);
	Q_UNUSED(id);
	Q_UNUSED(userParam);
	QString msg = QString::fromUtf8(message, length);
	switch(severity)
	{
		case GL_DEBUG_SEVERITY_HIGH:
			qCritical() << msg;
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			qCritical() << msg;
			break;
		case GL_DEBUG_SEVERITY_LOW:
			qCritical() << msg;
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			qInfo() << msg;
			break;
		default:
			qDebug() << msg;
			break;
	}
}

void ModelEditorView::undo()
{
	if(this->mUndoStack.size() == 0) {
		return;
	}
	this->mMesh = this->mUndoStack.pop();
	this->clearSelection();
	this->repaint();
}

void ModelEditorView::rotateFace(Face &face, RotateDir dir)
{
	for(int i = 0; i < 4; i++)
	{
		glm::ivec3 local = face.vertices[i].position - face.fulcrum;
		if(dir == Right)
		{
			std::swap(local.x, local.y);
			local.x = -local.x;
		}
		else
		{
			std::swap(local.x, local.y);
			local.y = -local.y;
		}
		face.vertices[i].position = face.fulcrum + local;
	}
}

void ModelEditorView::rotateLeft()
{
	Face * selection = this->getSelection();
	if(selection != nullptr) {
		this->rotateFace(*selection, Left);
	}
	if(this->hasInsertion()) {
		this->mSpriteToInsertRotation -= 1;
		if(this->mSpriteToInsertRotation < 0) {
			this->mSpriteToInsertRotation += 4;
		}
	}
	this->updateGizmos();
	this->repaint();
}

void ModelEditorView::rotateRight()
{
	Face * selection = this->getSelection();
	if(selection != nullptr) {
		this->rotateFace(*selection, Right);
	}
	if(this->hasInsertion()) {
		this->mSpriteToInsertRotation += 1;
		if(this->mSpriteToInsertRotation >= 4) {
			this->mSpriteToInsertRotation -= 4;
		}
	}
	this->updateGizmos();
	this->repaint();
}

void ModelEditorView::switchToTool(Tool tool)
{
	if(tool == Create) {
		qDebug() << "Invalid tool: Create." << "Use beginInsertSprite instead!";
		return;
	}
	this->mCurrentTool = tool;
}


void ModelEditorView::flipVertical()
{
	if(this->hasInsertion())
	{
		this->mSpriteToInsertFlipping = (Flipping)(this->mSpriteToInsertFlipping ^ Vertical);
	}
	if(this->hasSelection())
	{
		Face * face = this->getSelection();
		std::swap(face->vertices[0].uv, face->vertices[2].uv);
		std::swap(face->vertices[1].uv, face->vertices[3].uv);
	}
	this->repaint();
}

void ModelEditorView::flipHorizontal()
{
	if(this->hasInsertion())
	{
		this->mSpriteToInsertFlipping = (Flipping)(this->mSpriteToInsertFlipping ^ Horizontal);
	}
	if(this->hasSelection())
	{
		Face * face = this->getSelection();
		std::swap(face->vertices[0].uv, face->vertices[1].uv);
		std::swap(face->vertices[2].uv, face->vertices[3].uv);
	}
	this->repaint();
}

void ModelEditorView::deleteSelection()
{
	if(this->hasSelection() == false) {
		return;
	}

	this->meshIsAboutToChange();
	this->mMesh.faces.erase(this->mMesh.faces.begin() + this->mSelectedFace);
	this->meshChanged();

	this->clearSelection();
}

void ModelEditorView::getRay(int x, int y, glm::vec3 & origin, glm::vec3 & direction) const
{
	// unproject ray here
	glm::vec3 rayStart(
	            x,
	            this->height() - y - 1,
	            0.0f);
	glm::vec3 rayEnd(rayStart.x, rayStart.y, 0.8f);

	glm::vec4 viewport(0, 0, this->width(), this->height());

	rayStart = glm::unProject(
	            rayStart,
	            glm::mat4(),
	            this->matViewProj,
	            viewport);
	rayEnd = glm::unProject(
	            rayEnd,
	            glm::mat4(),
	            this->matViewProj,
	            viewport);

	direction = glm::normalize(rayEnd - rayStart);
	origin = rayStart;
}

glm::ivec3 ModelEditorView::raycastAgainstPlane(int x, int y) const
{
	glm::vec3 p0(this->mCameraFocus);
	glm::vec3 n(this->planeNormal());
	return this->raycastAgainstPlane(p0, n, x, y);
}

glm::ivec3 ModelEditorView::raycastAgainstPlane(glm::vec3 p0, glm::vec3 n, int x, int y) const
{
	glm::vec3 l, l0;
	this->getRay(x, y, l0, l);

	float dist = glm::dot(p0 - l0, n) / glm::dot(l, n);

	glm::vec3 pos(l0 + dist * l);

	pos = glm::vec3(this->mCameraFocus * this->planeNormal())
	        + glm::vec3(1 - this->planeNormal()) * pos;

	glm::ivec3 result(
	            floor0(pos.x),
	            floor0(pos.y),
	            floor0(pos.z));

	return result;
}

bool ModelEditorView::getFaceToInsert(Face & face)
{
	if(this->mSpriteToInsert.width() == 0 || this->mCurrentTool != Create) {
		return false;
	}

	glm::ivec3 normal, tangent, cotangent;
	this->getPlane(normal, tangent, cotangent);

	switch(this->mSpriteToInsertRotation % 4)
	{
		case 0:
			break;
		case 1:
			std::swap(tangent, cotangent);
			tangent = -tangent;
			break;
		case 2:
			tangent = -tangent;
			cotangent = -cotangent;
			break;
		case 3:
			std::swap(tangent, cotangent);
			cotangent = -cotangent;
			break;
	}

	face.normal = normal;
	face.fulcrum = this->mCursorPosition;

	face.vertices[0] = Vertex(
	            this->mCursorPosition,
	            C(this->mSpriteToInsert.bottomLeft() + QPoint(0,1)));
	face.vertices[1] = Vertex(
	            this->mCursorPosition
	            + this->mSpriteToInsert.width() * tangent,
	            C(this->mSpriteToInsert.bottomRight() + QPoint(1,1)));
	face.vertices[2] = Vertex(
	            this->mCursorPosition
	            + this->mSpriteToInsert.height() * cotangent,
	            C(this->mSpriteToInsert.topLeft()));
	face.vertices[3] = Vertex(
	            this->mCursorPosition
	            + this->mSpriteToInsert.width() * tangent
	            + this->mSpriteToInsert.height() * cotangent,
	            C(this->mSpriteToInsert.topRight() + QPoint(1,0)));

	if(this->mSpriteToInsertFlipping & Horizontal)
	{
		std::swap(face.vertices[0].uv, face.vertices[1].uv);
		std::swap(face.vertices[2].uv, face.vertices[3].uv);
	}
	if(this->mSpriteToInsertFlipping & Vertical)
	{
		std::swap(face.vertices[0].uv, face.vertices[2].uv);
		std::swap(face.vertices[1].uv, face.vertices[3].uv);
	}

	return true;
}

void ModelEditorView::updateGizmos()
{
	Face * sel = this->getSelection();
	if(sel == nullptr) {
		for(int i = 0; i < 5; i++) {
			mGizmoPositions[i] = QPoint(-10, -10);
		}
		return;
	}

	glm::vec4 viewport(0, 0, this->width(), this->height());

	glm::vec3 acc;
	for(int i = 0; i < 5; i++)
	{
		glm::vec3 origin;
		if(i < 4) {
			origin = glm::vec3(sel->vertices[i].position);
			acc += 0.25f * origin;
		}
		else {
			origin = acc;
		}

		glm::vec3 pos = glm::project(
		            origin,
		            glm::mat4(),
		            this->matViewProj,
		            viewport);

		// round smartly
		this->mGizmoPositions[i] = QPoint(pos.x, this->height() - pos.y - 1);

		qDebug() << "#" << i << pos << this->mGizmoPositions[i];
	}
}

////////////////////////////////////////////////////////////////////////////////

void ModelEditorView::initializeGL()
{
	if(gl3wInit() < 0) {
		qCritical() << "Failed to initialize OpenGL";
	}

	glDebugMessageCallback(&sysOpenGLDebug, nullptr);
	glEnable(GL_DEBUG_OUTPUT);

	struct { int major, minor; } version;
	glGetIntegerv(GL_MAJOR_VERSION, &version.major);
	glGetIntegerv(GL_MINOR_VERSION, &version.minor);
	qDebug() << "Initialized OpenGL" << version.major << version.minor;

	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);

	glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenBuffers(1, &this->vbuffer);
	glGenVertexArrays(1, &this->vao);

	glBindVertexArray(this->vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, this->vbuffer);

	glVertexAttribPointer(
	            0,
	            3,
	            GL_INT,
	            GL_FALSE,
	            sizeof(Vertex),
	            (void const * )offsetof(Vertex, position));
	glVertexAttribPointer(
	            1,
	            2,
	            GL_INT,
	            GL_FALSE,
	            sizeof(Vertex),
	            (void const * )offsetof(Vertex, uv));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	QFile vshdfile(":/glsl/vertex.glsl");
	if(vshdfile.open(QFile::ReadOnly) == false) {
		qCritical() << "Failed to open vertex shader file";
	}

	QFile fshdfile(":/glsl/fragment.glsl");
	if(fshdfile.open(QFile::ReadOnly) == false) {
		qCritical() << "Failed to open fragment shader file";
	}

	QByteArray vshader = vshdfile.readAll();
	QByteArray fshader = fshdfile.readAll();

	GLchar * vshadersource = vshader.data();
	GLchar * fshadersource = fshader.data();

	this->shader = glCreateProgram();
	GLuint g_VertHandle = glCreateShader(GL_VERTEX_SHADER);
	GLuint g_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(g_VertHandle, 1, &vshadersource, 0);
	glShaderSource(g_FragHandle, 1, &fshadersource, 0);
	glCompileShader(g_VertHandle);
	glCompileShader(g_FragHandle);
	glAttachShader(this->shader, g_VertHandle);
	glAttachShader(this->shader, g_FragHandle);
	glLinkProgram(this->shader);

	this->locMatTransform = glGetUniformLocation(this->shader, "matTransform");
	this->locTexDiffuse   = glGetUniformLocation(this->shader, "texDiffuse");
	this->locVecTint      = glGetUniformLocation(this->shader, "vecTint");
	this->locIAlphaTest   = glGetUniformLocation(this->shader, "iAlphaTest");

	this->mPixel.reset(new QOpenGLTexture(QImage(":/data/pixel.png")));

	if(this->hasMesh())
	{
		this->mTexture.reset(new QOpenGLTexture(this->mMesh.texture, QOpenGLTexture::DontGenerateMipMaps));
		this->mTexture->setWrapMode(QOpenGLTexture::ClampToBorder);
		this->mTexture->setBorderColor(1.0f, 0.0f, 1.0f, 1.0f);
		this->mTexture->setMinificationFilter(QOpenGLTexture::Nearest);
		this->mTexture->setMagnificationFilter(QOpenGLTexture::Nearest);
	}
}

void ModelEditorView::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);
}

void ModelEditorView::paintGL()
{
	glm::vec3 cameraOffset(
	            sin(this->mPan) * cos(this->mTilt),
	            sin(this->mTilt),
	            cos(this->mPan) * cos(this->mTilt));

	this->mCameraPosition = glm::vec3(this->mCameraFocus) - this->mZoom * cameraOffset;

	auto matProj = glm::perspectiveFov(
	            glm::radians(60.0f),
	            float(this->width()),
	            float(this->height()),
	            0.1f,
	            1000.0f);
	auto matView = glm::lookAt(
	            this->mCameraPosition,
	            glm::vec3(this->mCameraFocus),
	            glm::vec3(0, 1, 0));

	this->matViewProj = matProj * matView;

	// glm::mat4 matInvViewProj = glm::inverse(matViewProj);

	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	std::vector<Vertex> vertices;

	glBindVertexArray(this->vao);
	glUseProgram(this->shader);
	glActiveTexture(GL_TEXTURE0);

	glUniformMatrix4fv(this->locMatTransform, 1, GL_FALSE, &this->matViewProj[0][0]);
	glUniform1i(this->locTexDiffuse, 0);
	glUniform1i(this->locIAlphaTest, 0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glm::ivec3 normal, tangent, cotangent;
	this->getPlane(normal, tangent, cotangent);

	{ // Render a ground plane
		glDepthMask(GL_TRUE);
		this->mPixel->bind();

		vertices.clear();
		vertices.emplace_back(glm::ivec3(-256, 0, -256));
		vertices.emplace_back(glm::ivec3( 256, 0, -256));
		vertices.emplace_back(glm::ivec3(-256, 0,  256));
		vertices.emplace_back(glm::ivec3( 256, 0,  256));

		glBindBuffer(GL_ARRAY_BUFFER, this->vbuffer);
		glBufferData(
		            GL_ARRAY_BUFFER,
		            sizeof(Vertex) * vertices.size(),
		            vertices.data(),
		            GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glUniform1i(this->locIAlphaTest, 0);
		glUniform4f(this->locVecTint, 0.28f, 0.54f, 0.28f, 1.0f);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size());
	}

	{ // Render the fitting grid
		glDepthMask(GL_TRUE);
		this->mPixel->bind();

		vertices.clear();
		for(int u = -10; u <= 10; u++)
		{
			for(int v = -10; v <= 10; v++)
			{
				vertices.emplace_back(this->mCameraFocus + 16 * u * tangent + 16 * v * cotangent);
				vertices.emplace_back(this->mCameraFocus + 16 * u * tangent - 16 * v * cotangent);

				vertices.emplace_back(this->mCameraFocus + 16 * u * cotangent + 16 * v * tangent);
				vertices.emplace_back(this->mCameraFocus + 16 * u * cotangent - 16 * v * tangent);
			}
		}

		glBindBuffer(GL_ARRAY_BUFFER, this->vbuffer);
		glBufferData(
		            GL_ARRAY_BUFFER,
		            sizeof(Vertex) * vertices.size(),
		            vertices.data(),
		            GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glUniform1i(this->locIAlphaTest, 0);
		glUniform4f(this->locVecTint, 1.0f, 1.0f, 1.0f, 0.6f);

		glLineWidth(1.0f);
		glDrawArrays(GL_LINES, 0, vertices.size());
	}

	Face faceToInsert;
	bool hasFaceToInsert = this->getFaceToInsert(faceToInsert);
	if((this->mTexture != nullptr) || ((this->mMesh.faces.size() > 0) || hasFaceToInsert))
	{ // Render the 3D model
		glDepthMask(GL_TRUE);
		this->mTexture->bind();
		vertices.clear();
		vertices.reserve(this->mMesh.faces.size() * 6);
		for(Face & face : this->mMesh.faces)
		{
			vertices.push_back(face.vertices[0]);
			vertices.push_back(face.vertices[1]);
			vertices.push_back(face.vertices[3]);

			vertices.push_back(face.vertices[0]);
			vertices.push_back(face.vertices[2]);
			vertices.push_back(face.vertices[3]);
		}

		if(hasFaceToInsert)
		{ // The last sprite is the currently inserted sprite.

			vertices.push_back(faceToInsert.vertices[0]);
			vertices.push_back(faceToInsert.vertices[1]);
			vertices.push_back(faceToInsert.vertices[3]);

			vertices.push_back(faceToInsert.vertices[0]);
			vertices.push_back(faceToInsert.vertices[2]);
			vertices.push_back(faceToInsert.vertices[3]);
		}

		if(vertices.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, this->vbuffer);
			glBufferData(
			            GL_ARRAY_BUFFER,
			            sizeof(Vertex) * vertices.size(),
			            vertices.data(),
			            GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glUniform1i(this->locIAlphaTest, 1);
			glUniform4f(this->locVecTint, 1.0f, 1.0f, 1.0f, 1.0f);
			glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		}
	}

	if(this->hasSelection())
	{ // Render the selection outline
		glDepthMask(GL_TRUE);
		this->mPixel->bind();

		Face * sel = this->getSelection();

		vertices.clear();

		vertices.push_back(sel->vertices[0]);
		vertices.push_back(sel->vertices[1]);

		vertices.push_back(sel->vertices[1]);
		vertices.push_back(sel->vertices[3]);

		vertices.push_back(sel->vertices[0]);
		vertices.push_back(sel->vertices[2]);

		vertices.push_back(sel->vertices[2]);
		vertices.push_back(sel->vertices[3]);

		vertices.push_back(glm::ivec3(0.25f * glm::vec3(
		                                  sel->vertices[0].position
		                              + sel->vertices[1].position
		                   + sel->vertices[2].position
		        + sel->vertices[3].position)));

		glBindBuffer(GL_ARRAY_BUFFER, this->vbuffer);
		glBufferData(
		            GL_ARRAY_BUFFER,
		            sizeof(Vertex) * vertices.size(),
		            vertices.data(),
		            GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glUniform1i(this->locIAlphaTest, 0);

		glDisable(GL_DEPTH_TEST);

		glUniform4f(this->locVecTint, 1.0f, 0.0f, 0.0f, 1.0f);
		glDrawArrays(GL_LINES, 0, vertices.size() - 1); // Ignore center point ;)

		glPointSize(8.0f);
		glUniform4f(this->locVecTint, 0.0f, 0.0f, 1.0f, 1.0f);
		glDrawArrays(GL_POINTS, 0, vertices.size());

		glEnable(GL_DEPTH_TEST);
	}

	glDepthMask(GL_TRUE);
}