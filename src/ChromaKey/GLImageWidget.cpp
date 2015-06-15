
#include "GLImageWidget.h"
#include <QMouseEvent>
#include <QCoreApplication>
#include <QMessageBox>
#include <math.h>
#include <iostream>



#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1



GLImageWidget::GLImageWidget(QWidget *parent)
	: QOpenGLWidget(parent),
	threshold(0.1f),
	slope(0.05f),
	useYCbCr(true),
	useCrominance(false),
	maskOutput(false),
	chromaKeyEnabled(false)
{
	resetMinMaxColors();

#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
	qDebug() << QT_VERSION;
	qDebug() << "qt > 5.4";
#endif
	
}


GLImageWidget::~GLImageWidget()
{
	cleanup();
}

void GLImageWidget::enableChromaKey(bool enable)
{
	this->chromaKeyEnabled = enable;
	update();
}

void GLImageWidget::setCrominance(bool use_crominance)
{
	this->useCrominance = use_crominance;
	update();
}


void GLImageWidget::setMaskOutput(bool mask_output)
{
	this->maskOutput = mask_output ? 1.0f : 0.0f;;
	update();
}

void GLImageWidget::setThreshold(int t)
{
	this->threshold = float(t) * 0.01f;	// converting range from [0,100] to [0.0,1.0]
	this->update();
}

void GLImageWidget::setSlope(int s)
{
	this->slope = float(s) * 0.01f; // converting range from [0,100] to [0.0,1.0]
	this->update();
}

void GLImageWidget::setImage(const QImage& inputImage)
{
	this->image = inputImage;
	m_texture.clear();
	m_texture = QSharedPointer<QOpenGLTexture>(new QOpenGLTexture(inputImage.mirrored()));
	m_texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
	m_texture->setMagnificationFilter(QOpenGLTexture::Linear);

	resetMinMaxColors();

	emit colorChanged();
}


void GLImageWidget::cleanup()
{
	makeCurrent();
	m_vbo.destroy();
	
	m_texture.clear();

	doneCurrent();
}


void GLImageWidget::initializeGL()
{
	this->initializeOpenGLFunctions();

	// get context opengl-version
	qDebug() << "Widget OpenGl: " << format().majorVersion() << "." << format().minorVersion();
	qDebug() << "Context valid: " << context()->isValid();
	qDebug() << "Really used OpenGl: " << context()->format().majorVersion() << "." << context()->format().minorVersion();
	qDebug() << "OpenGl information: VENDOR:       " << (const char*)glGetString(GL_VENDOR);
	qDebug() << "                    RENDERDER:    " << (const char*)glGetString(GL_RENDERER);
	qDebug() << "                    VERSION:      " << (const char*)glGetString(GL_VERSION);
	qDebug() << "                    GLSL VERSION: " << (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);


	this->initShaders();

	// Create a vertex array object. In OpenGL ES 2.0 and OpenGL 2.x
	// implementations this is optional and support may not be present
	// at all. Nonetheless the below code works in all cases and makes
	// sure there is a VAO when one is needed.
	m_vao.create();
	QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

	typedef struct
	{
		float xyz[3];
		float uv[2];
	} VertexUV;

	VertexUV Vertices[] =
	{
		{ { -1.0f, -1.0f, 0.0f}, { 0.0f, 0.0f} },
		{ { -1.0f, 1.0f, 0.0f}, { 0.0f, 1.0f} },
		{ { 1.0f, 1.0f, 0.0f}, { 1.0f, 1.0f} },
		{ { 1.0f, -1.0f, 0.0f}, { 1.0f, 0.0f} }
	};

	GLenum ErrorCheckValue = glGetError();
	const size_t BufferSize = sizeof(Vertices);
	const size_t VertexSize = sizeof(Vertices[0]);
	const size_t UVOffset = sizeof(Vertices[0].xyz);

	// Setup our vertex buffer object.
	m_vbo.create();
	m_vbo.bind();
	m_vbo.allocate(Vertices, BufferSize);

	m_vbo.bind();

	QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
	f->glClearColor(0, 0, 0, 0);
	f->glEnableVertexAttribArray(0);
	f->glEnableVertexAttribArray(1);
	f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VertexSize, 0);
	f->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, VertexSize, reinterpret_cast<void *>(UVOffset));

	m_vbo.release();
	m_program->release();
}

void GLImageWidget::initShaders()
{
	QString simpleTexVertShaderFile = "../../../shaders/SimpleTexture.vert";
	QString simpleTexFragShaderFile = "../../../shaders/SimpleTexture.frag";
	QString chromaKeyVertShaderFile = "../../../shaders/SimpleTexture.vert";
	//QString chromaKeyFragShaderFile = "../../../shaders/BilateralFilter.frag";
	QString chromaKeyFragShaderFile = "../../../shaders/GuidedImageFilter.frag";



	QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
	const char *vsrc =
		"attribute highp vec4 in_Vertex;\n"
		"attribute mediump vec4 in_TexCoord;\n"
		"varying mediump vec4 texc;\n"
		"void main(void)\n"
		"{\n"
		"    gl_Position = in_Vertex;\n"
		"    texc = in_TexCoord;\n"
		"}\n";
	vshader->compileSourceCode(vsrc);


	QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
	const char *fsrc =
		"uniform sampler2D in_Texture;\n"
		"varying mediump vec4 texc;\n"
		"void main(void)\n"
		"{\n"
		"    gl_FragColor = texture2D(in_Texture, texc.st);\n"
		"}\n";
	fshader->compileSourceCode(fsrc);
	

	QOpenGLShader *simpleTexVertShader = new QOpenGLShader(QOpenGLShader::Vertex, this);
	QOpenGLShader *simpleTexFragShader = new QOpenGLShader(QOpenGLShader::Fragment, this);
	if (!simpleTexVertShader->compileSourceFile(simpleTexVertShaderFile))
	{
		QMessageBox::critical(this, tr("GLImageWidget"),
			QString("Could not load vertex shader file.\n<" + simpleTexVertShaderFile + ">."),
			QMessageBox::Ok);
	}
	if (!simpleTexFragShader->compileSourceFile(simpleTexFragShaderFile))
	{
		QMessageBox::critical(this, tr("GLImageWidget"),
			QString("Could not load fragment shader file.\n<" + simpleTexFragShaderFile + ">."),
			QMessageBox::Ok);
	}


	QOpenGLShader *chromaKeyVertShader = new QOpenGLShader(QOpenGLShader::Vertex, this);
	QOpenGLShader *chromaKeyFragShader = new QOpenGLShader(QOpenGLShader::Fragment, this);
	if (!chromaKeyVertShader->compileSourceFile(chromaKeyVertShaderFile))
	{
		QMessageBox::critical(this, tr("GLImageWidget"),
			QString("Could not load vertex shader file.\n<" + chromaKeyVertShaderFile + ">."),
			QMessageBox::Ok);
	}
	if (!chromaKeyFragShader->compileSourceFile(chromaKeyFragShaderFile))
	{
		QMessageBox::critical(this, tr("GLImageWidget"),
			QString("Could not load fragment shader file.\n<" + chromaKeyFragShaderFile + ">."),
			QMessageBox::Ok);
	}


	m_program = &m_shaderProgram[0];
	m_program->addShader(simpleTexVertShader);
	m_program->addShader(simpleTexFragShader);
	m_program->bindAttributeLocation("in_Vertex", PROGRAM_VERTEX_ATTRIBUTE);
	m_program->bindAttributeLocation("in_TexCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
	m_program->link();
	m_program->bind();
	m_program->setUniformValue("in_Texture", 0);
	
	m_program = &m_shaderProgram[1];
	m_program->addShader(chromaKeyVertShader);
	m_program->addShader(chromaKeyFragShader);
	m_program->bindAttributeLocation("in_Vertex", PROGRAM_VERTEX_ATTRIBUTE);
	m_program->bindAttributeLocation("in_TexCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
	m_program->link();
	m_program->bind();
	m_program->setUniformValue("in_Texture", 0);

	m_program = &m_shaderProgram[1];

	threshold = 8;
	slope = 0.4f;
}

void GLImageWidget::paintGL()
{
	if (!m_texture.isNull())
		m_texture->bind();

	QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

	if (chromaKeyEnabled)
	{
		m_program = &m_shaderProgram[1];
		m_program->bind();
#if 0
		m_program->setUniformValue("lowColor", minColor);
		m_program->setUniformValue("highColor", maxColor);
		m_program->setUniformValue("keyColor", keyColor);
		m_program->setUniformValue("useYCbCr", useYCbCr);
		m_program->setUniformValue("useCrominance", useCrominance);
		m_program->setUniformValue("maskOutput", maskOutput);
		m_program->setUniformValue("threshold", threshold);
		m_program->setUniformValue("slope", this->slope);
		m_program->setUniformValue("alphaLowPass", 1.0f);
		m_program->setUniformValue("alphaHighPass", 0.0f);
#endif
		int windowSize = threshold * 10;
		float E = slope;
		std::cout << windowSize << " , " << E << std::endl;
		m_program->setUniformValue("windowSize", windowSize);
		m_program->setUniformValue("E", E);
	}
	else
	{
		m_program = &m_shaderProgram[0];
		m_program->bind();
	}
	

	QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
	f->glDrawArrays(GL_QUADS, 0, 4);

	m_program->release();
}


void GLImageWidget::resizeGL(int w, int h)
{
	QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
	f->glViewport(0, 0, this->width(), this->height());
}


void GLImageWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::MouseButton::MiddleButton)
		clickBegin = event->pos();
}


void GLImageWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::MouseButton::MiddleButton)
	{
		clickEnd = event->pos();
		this->computeColorRange();
		emit colorChanged();
	}
	
	if (event->button() == Qt::MouseButton::RightButton)
	{
		useYCbCr = !useYCbCr;
		qDebug() << useYCbCr;
	}
	/*if (event->button() == Qt::MouseButton::RightButton)
	{
		int px = float(event->pos().x()) / this->width() * this->image.width();
		int py = float(event->pos().y()) / this->height() * this->image.height();
		QColor pixel(this->image.pixel(px, py));
		keyColor = (QVector3D(float(pixel.red()) / 255.0f, float(pixel.green()) / 255.0f, float(pixel.blue()) / 255.0f));
		minColor = maxColor = keyColor;
	}*/

	this->update();
}


void GLImageWidget::mouseMoveEvent(QMouseEvent *event)
{
}


void GLImageWidget::computeColorRange()
{
	resetMinMaxColors();

	float x0, x1, y0, y1;
	
	if (this->clickBegin.x() < this->clickEnd.x())
	{
		x0 = this->clickBegin.x();
		x1 = this->clickEnd.x();
	}
	else
	{
		x0 = this->clickEnd.x();
		x1 = this->clickBegin.x();
	}

	if (this->clickBegin.y() < this->clickEnd.y())
	{
		y0 = this->clickBegin.y();
		y1 = this->clickEnd.y();
	}
	else
	{
		y0 = this->clickEnd.y();
		y1 = this->clickBegin.y();
	}

	int px0 = float(x0) / this->width() * this->image.width();
	int px1 = float(x1) / this->width() * this->image.width();
	int py0 = float(y0) / this->height() * this->image.height();
	int py1 = float(y1) / this->height() * this->image.height();

	for (int px = px0; px <= px1; ++px)
	{
		for (int py = py0; py <= py1; ++py)
		{
			QColor pixel(this->image.pixel(px, py));

			//QVector3D p(RGB2YCbCr(QVector3D( float(pixel.red()) / 255.0f, float(pixel.green()) / 255.0f, float(pixel.blue()) / 255.0f)));
			QVector3D p(QVector3D(float(pixel.red()) / 255.0f, float(pixel.green()) / 255.0f, float(pixel.blue()) / 255.0f));
			if (useYCbCr)
			{
				p = RGB2YCbCr(p);
			}

			for (int i = 0; i < 3; ++i)
			{
				if (p[i] < minColor[i])
					minColor[i] = p[i];

				if (p[i] > maxColor[i])
					maxColor[i] = p[i];
			}
		}
	}
}

void GLImageWidget::resetMinMaxColors()
{
	if (useYCbCr)
	{
		minColor = RGB2YCbCr(QVector3D(1, 1, 1));
		maxColor = RGB2YCbCr(QVector3D(0, 0, 0));
		keyColor = RGB2YCbCr(QVector3D(0, 0, 0));
	}
	else
	{
		minColor = QVector3D(1, 1, 1);
		maxColor = QVector3D(0, 0, 0);
		keyColor = QVector3D(0, 0, 0);
	}
}

QColor GLImageWidget::getMinColorRGB() const
{
	QVector3D v = minColor;
	if (useYCbCr)
		v = YCbCr2RGB(minColor);
	return QColor(int(v[0] * 255.0f), int(v[1] * 255.0f), int(v[2] * 255.0f));
}

QColor GLImageWidget::getMaxColorRGB() const
{
	QVector3D v = maxColor;
	if (useYCbCr)
		v = YCbCr2RGB(maxColor);
	return QColor(int(v[0] * 255.0f), int(v[1] * 255.0f), int(v[2] * 255.0f));
}

QVector3D GLImageWidget::RGB2YCbCr(QVector3D c) const
{
	float Y = 0.2989f * c[0] + 0.5866f * c[1] + 0.1145f * c[2];
	float Cb = -0.1687f * c[0] -0.3313f * c[1] + 0.5000f * c[2];
	float Cr = 0.5000f * c[0] - 0.4184f * c[1] - 0.0816f * c[2];

	return QVector3D(Y, Cb, Cr);
}

QVector3D GLImageWidget::YCbCr2RGB(QVector3D c) const
{
	float r = c[0] + 0.0000f * c[1] + 1.4022f * c[2];
	float g = c[0] - 0.3456f * c[1] - 0.7145f * c[2];
	float b = c[0] + 1.7710f * c[1] + 0.0000f * c[2];

	return QVector3D(r, g, b);
}