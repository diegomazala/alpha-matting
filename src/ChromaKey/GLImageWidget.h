#ifndef __GL_IMAGE_WIDGET_H__
#define __GL_IMAGE_WIDGET_H__

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QMatrix4x4>
#include <QSharedPointer>
#include <QOpenGLShaderProgram>
#include <QImage>



class GLImageWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	GLImageWidget(QWidget *parent = 0);
	~GLImageWidget();

	QSize minimumSizeHint() const Q_DECL_OVERRIDE{ return QSize(50, 50); };
	QSize sizeHint() const Q_DECL_OVERRIDE{ return QSize(1280, 400); };

	QColor getMinColorRGB() const;
	QColor getMaxColorRGB() const;

public slots:
	void cleanup();
	void setImage(const QImage& image);
	void enableChromaKey(bool enable);
	void setCrominance(bool use_crominance);
	void setMaskOutput(bool mask_output);
	void setThreshold(int t);
	void setSlope(int s);


signals:
	void colorChanged();

protected:
	void initializeGL() Q_DECL_OVERRIDE;
	void paintGL() Q_DECL_OVERRIDE;
	void resizeGL(int width, int height) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

	void initShaders();

	void resetMinMaxColors();
	void computeColorRange();
	QVector3D RGB2YCbCr(QVector3D c) const;
	QVector3D YCbCr2RGB(QVector3D c) const;

private:
	QOpenGLVertexArrayObject m_vao;
	QOpenGLBuffer m_vbo;
	QSharedPointer<QOpenGLTexture> m_texture;
	QOpenGLShaderProgram m_shaderProgram[2];
	QOpenGLShaderProgram* m_program;
	
	QImage image;

	QPoint clickBegin;
	QPoint clickEnd;
	QVector3D minColor;
	QVector3D maxColor;
	QVector3D keyColor;
	bool useYCbCr;
	bool useCrominance;
	bool maskOutput;
	float threshold;
	float slope;
	bool chromaKeyEnabled;
};


#endif // __GL_IMAGE_WIDGET_H__
