
#include "GLImageWidget.h"
#include <iostream>


GLImageWidget::GLImageWidget(QWidget *parent)
	: QOpenGLWidget(parent),
	filterId(0),
	imgPassthrough(this),
	bilateralFilter(this),
	guidedFilter(this)
{

	// loop call update
	// QTimer *timer = new QTimer(this);
	// connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	// timer->start(33);
}


GLImageWidget::~GLImageWidget()
{
	cleanup();
}


void GLImageWidget::setImage(const QImage& inputImage)
{
	makeCurrent();
	imgPassthrough.setImage(inputImage);
	bilateralFilter.setImage(inputImage);
	guidedFilter.setImage(inputImage);
	update();
}



void GLImageWidget::setImage(const QString& fileImage)
{
	setImage(QImage(fileImage));
}


void GLImageWidget::cleanup()
{
	this->makeCurrent();
	imgPassthrough.cleanup();
	bilateralFilter.cleanup();
	guidedFilter.cleanup();
	this->doneCurrent();
}


void GLImageWidget::initializeGL()
{
	this->initializeOpenGLFunctions();

#if 0
	// get context opengl-version
	qDebug() << "Widget OpenGl: " << format().majorVersion() << "." << format().minorVersion();
	qDebug() << "Context valid: " << context()->isValid();
	qDebug() << "Really used OpenGl: " << context()->format().majorVersion() << "." << context()->format().minorVersion();
	qDebug() << "OpenGL information: VENDOR:       " << (const char*)glGetString(GL_VENDOR);
	qDebug() << "                    RENDERDER:    " << (const char*)glGetString(GL_RENDERER);
	qDebug() << "                    VERSION:      " << (const char*)glGetString(GL_VERSION);
	qDebug() << "                    GLSL VERSION: " << (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
#endif

	imgPassthrough.initialize();
	bilateralFilter.initialize();
	guidedFilter.initialize();
}




void GLImageWidget::paintGL()
{
	switch (filterId)
	{
	case 2:											// Filter::Guided
	{
		guidedFilter.plot(width(), height());
		break;
	}
	case 1:											// Filter::Bilateral
		bilateralFilter.render();
		break;

	case 0:											// Filter::None
	default:
		imgPassthrough.render();
		break;
	}

}


void GLImageWidget::resizeGL(int w, int h)
{
	QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
	f->glViewport(0, 0, this->width(), this->height());

	std::cout << "resize: " << w << ", " << h << std::endl;
}


void GLImageWidget::setFilter(int filter)
{
	filterId = filter;

	switch (filterId)
	{
	case 2: // Filter::Guided
		break;

	case 1: // Filter::Bilateral
		break;

	case 0: // Filter::None
	default:
		break;
	}
	update();
}


void GLImageWidget::onBilateralFilterChanged(int kernel_size, float spatial_radius, float intensity_radius)
{
	makeCurrent();	// to rebuild texture
	bilateralFilter.setParameters(kernel_size, spatial_radius, intensity_radius);
	update();
}


void GLImageWidget::onGuidedFilterChanged(int kernel_size, float regularization_param, bool use_guide_image, QImage guide_image)
{
	makeCurrent();	// to rebuild texture
	guidedFilter.setParameters(kernel_size, regularization_param, use_guide_image, guide_image);
	guidedFilter.run();
	update();
}