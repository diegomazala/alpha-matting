#ifndef _GL_BILATERAL_FILTER_H_
#define _GL_BILATERAL_FILTER_H_


#include "GLImageProcessor.h"



class GLBilateralFilter : public GLImageProcessor
{
	Q_OBJECT

public:
	GLBilateralFilter(QObject *parent = 0);
	~GLBilateralFilter();


public slots:
	virtual void initialize();
	virtual void render();

	void setParameters(int kernel_size, float spatial_radius, float intensity_radius);

protected:	// methods

	QOpenGLShaderProgram* buildProgram() const;

protected : // attributes
	
	int kernelSize;
	float spatialRadius;
	float intensityRadius;
};


#endif // _GL_BILATERAL_FILTER_H_
