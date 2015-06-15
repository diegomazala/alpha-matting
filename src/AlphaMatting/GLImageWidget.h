#ifndef __GL_IMAGE_WIDGET_H__
#define __GL_IMAGE_WIDGET_H__

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QScopedPointer>
#include <QOpenGLShaderProgram>
#include <QImage>
#include "GLImageProcessor.h"
#include "GLBilateralFilter.h"
#include "GLGuidedImageFilter.h"



class GLImageWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:

	GLImageWidget(QWidget *parent = 0);
	~GLImageWidget();

	QSize minimumSizeHint() const Q_DECL_OVERRIDE{ return QSize(50, 50); };
	QSize sizeHint() const Q_DECL_OVERRIDE{ return QSize(1280, 720); };

	

public slots:
	virtual void cleanup();
	void setImage(const QImage& image);
	void setImage(const QString& fileimage);
	void setFilter(int filter);
	void onBilateralFilterChanged(int kernel_size, float spatial_radius, float intensity_radius);
	void onGuidedFilterChanged(int kernel_size, float regularization_param, bool use_guide_image, QImage guide_image);

protected:	// methods

	virtual void initializeGL() Q_DECL_OVERRIDE;
	virtual void paintGL() Q_DECL_OVERRIDE;
	virtual void resizeGL(int width, int height) Q_DECL_OVERRIDE;

protected : // attributes
	
	int filterId;	
	GLImageProcessor	imgPassthrough;
	GLBilateralFilter	bilateralFilter;
	GLGuidedImageFilter	guidedFilter;
};


#endif // __GL_IMAGE_WIDGET_H__
