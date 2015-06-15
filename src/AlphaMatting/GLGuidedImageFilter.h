#ifndef _GL_GUIDED_FILTER_H_
#define _GL_GUIDED_FILTER_H_

#include "GLImageProcessor.h"

class GLGuidedImageFilter : public GLImageProcessor
{
	Q_OBJECT

public:
	GLGuidedImageFilter(QObject *parent = 0);
	~GLGuidedImageFilter();
	
public slots:
	virtual void initialize();
	virtual void render();
	virtual void run();
	virtual void plot(int w, int h);
	virtual void cleanup();
	void setParameters(int radius_size, float regularization_param, bool use_guide_image, QImage guide_image);
	void setImage(const QImage& image);
	

protected:	// methods

	QOpenGLShaderProgram* buildProgramPass(int index) const;
	void buildGuideTexture();

protected : // attributes
	
	enum
	{
		PASS_MEAN_P,
		PASS_MEAN_I,
		PASS_EQ5,
		PASS_EQ6,
		PASS_EQ8,
		FILTER_TOTAL
	};

	const int filterPassCount = 3;

	int radius;
	float eps;
	bool useGuideImage;
	QImage guideImage;
	QSharedPointer<QOpenGLTexture> guideTexture;

	QScopedPointer<QOpenGLShaderProgram> programPass[FILTER_TOTAL];

	QSharedPointer<QOpenGLFramebufferObject> fboPass[FILTER_TOTAL];
};


#endif // _GL_BILATERAL_FILTER_H_
