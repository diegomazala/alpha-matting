#include "GLGuidedImageFilter.h"
#include <QMessageBox>
#include <QFileInfo>
#include <iostream>

GLGuidedImageFilter::GLGuidedImageFilter(QObject *parent)
	: GLImageProcessor(parent),
	radius(9),
	eps(0.4f),
	useGuideImage(false),
	guideImage(1, 1, QImage::Format_Indexed8)
{
	guideImage.fill(0);
}


GLGuidedImageFilter::~GLGuidedImageFilter()
{
}

void GLGuidedImageFilter::cleanup()
{
	guideTexture.clear();
}


void GLGuidedImageFilter::setImage(const QImage& inputImage)
{
	
	image = inputImage;
	//renderFbo.reset(new QOpenGLFramebufferObject(image.size(), renderFbo->format()));

	for (int i = 0; i < FILTER_TOTAL; ++i)
		fboPass[i].reset(new QOpenGLFramebufferObject(image.size(), fboPass[i]->format()));

	buildTexture();
}

void GLGuidedImageFilter::setParameters(int radius_size, float regularization_param, bool use_guide_image, QImage guide_image)
{
	radius = radius_size;
	eps = regularization_param;
	//useGuideImage = use_guide_image;		
	//guideImage = guide_image;
	guideImage = image;
	buildGuideTexture();
}


void GLGuidedImageFilter::buildGuideTexture()
{
	guideTexture.reset(new QOpenGLTexture(guideImage.mirrored()));
	guideTexture->setWrapMode(QOpenGLTexture::WrapMode::ClampToEdge);
	guideTexture->setMinificationFilter(QOpenGLTexture::Nearest);
	guideTexture->setMagnificationFilter(QOpenGLTexture::Nearest);
}


void GLGuidedImageFilter::initialize()
{
	GLImageProcessor::initialize();
	
	guideTexture.reset(new QOpenGLTexture(image.mirrored()));
	guideTexture->setWrapMode(QOpenGLTexture::WrapMode::ClampToEdge);
	guideTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
	guideTexture->setMagnificationFilter(QOpenGLTexture::Linear);

	QOpenGLFramebufferObjectFormat format;
	format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

	for (int i = 0; i < FILTER_TOTAL; ++i)
	{
		programPass[i].reset(buildProgramPass(i));
		programPass[i]->release();

		fboPass[i].reset(new QOpenGLFramebufferObject(image.size(), format));
	}
}





void GLGuidedImageFilter::render()
{
	QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
	vao.bind();
	
	program->bind();

	f->glActiveTexture(GL_TEXTURE0);
	if (!texture.isNull())
		texture->bind();

	f->glActiveTexture(GL_TEXTURE1);
	if (!guideTexture.isNull())
		guideTexture->bind();

	program->setUniformValue("radius", radius);
	program->setUniformValue("eps", eps);

	f->glDrawArrays(GL_QUADS, 0, 4);
	program->release();

	vao.release();
}



void GLGuidedImageFilter::run()
{
	fboPass[PASS_MEAN_P]->bind();															// Mean Pass
	{
		QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
		vao.bind();

		programPass[PASS_MEAN_P]->bind();

		f->glActiveTexture(GL_TEXTURE0);
		texture->bind();

		programPass[PASS_MEAN_P]->setUniformValue("radius", radius);

		f->glViewport(0, 0, fboPass[PASS_MEAN_P]->width(), fboPass[PASS_MEAN_P]->height());
		f->glDrawArrays(GL_QUADS, 0, 4);
		programPass[PASS_MEAN_P]->release();

		vao.release();
	}
	fboPass[PASS_MEAN_P]->bindDefault();

	fboPass[PASS_MEAN_I]->bind();															// Mean Pass
	{
		QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
		vao.bind();

		programPass[PASS_MEAN_I]->bind();

		f->glActiveTexture(GL_TEXTURE0);
		guideTexture->bind();

		programPass[PASS_MEAN_I]->setUniformValue("radius", radius);

		f->glViewport(0, 0, fboPass[PASS_MEAN_I]->width(), fboPass[PASS_MEAN_I]->height());
		f->glDrawArrays(GL_QUADS, 0, 4);
		programPass[PASS_MEAN_I]->release();

		vao.release();
	}
	fboPass[PASS_MEAN_I]->bindDefault();


	fboPass[PASS_EQ5]->bind();															// Mean Pass
	{
		QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
		vao.bind();

		programPass[PASS_EQ5]->bind();

		f->glActiveTexture(GL_TEXTURE0);
		texture->bind();

		f->glActiveTexture(GL_TEXTURE1);
		guideTexture->bind();

		f->glActiveTexture(GL_TEXTURE2);
		f->glBindTexture(GL_TEXTURE_2D, fboPass[PASS_MEAN_P]->texture());

		f->glActiveTexture(GL_TEXTURE3);
		f->glBindTexture(GL_TEXTURE_2D, fboPass[PASS_MEAN_I]->texture());

		programPass[PASS_EQ5]->setUniformValue("radius", radius);
		programPass[PASS_EQ5]->setUniformValue("eps", eps);

		f->glViewport(0, 0, fboPass[PASS_EQ5]->width(), fboPass[PASS_EQ5]->height());
		f->glDrawArrays(GL_QUADS, 0, 4);
		programPass[PASS_EQ5]->release();

		vao.release();
	}
	fboPass[PASS_EQ5]->bindDefault();


	fboPass[PASS_EQ6]->bind();															// Mean Pass
	{
		QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
		vao.bind();

		programPass[PASS_EQ6]->bind();

		f->glActiveTexture(GL_TEXTURE0);
		f->glBindTexture(GL_TEXTURE_2D, fboPass[PASS_MEAN_P]->texture());

		f->glActiveTexture(GL_TEXTURE1);
		f->glBindTexture(GL_TEXTURE_2D, fboPass[PASS_MEAN_I]->texture());

		f->glActiveTexture(GL_TEXTURE1);
		f->glBindTexture(GL_TEXTURE_2D, fboPass[PASS_EQ5]->texture());

		programPass[PASS_EQ5]->setUniformValue("radius", radius);

		f->glViewport(0, 0, fboPass[PASS_EQ6]->width(), fboPass[PASS_EQ6]->height());
		f->glDrawArrays(GL_QUADS, 0, 4);
		programPass[PASS_EQ6]->release();

		vao.release();
	}
	fboPass[PASS_EQ6]->bindDefault();

	fboPass[PASS_EQ8]->bind();															// Mean Pass
	{
		QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
		vao.bind();

		programPass[PASS_EQ8]->bind();

		f->glActiveTexture(GL_TEXTURE0);
		guideTexture->bind();

		f->glActiveTexture(GL_TEXTURE1);
		f->glBindTexture(GL_TEXTURE_2D, fboPass[PASS_EQ5]->texture());

		f->glActiveTexture(GL_TEXTURE2);
		f->glBindTexture(GL_TEXTURE_2D, fboPass[PASS_EQ6]->texture());

		programPass[PASS_EQ5]->setUniformValue("radius", radius);

		f->glViewport(0, 0, fboPass[PASS_EQ8]->width(), fboPass[PASS_EQ8]->height());
		f->glDrawArrays(GL_QUADS, 0, 4);
		programPass[PASS_EQ8]->release();

		vao.release();
	}
	fboPass[PASS_EQ8]->bindDefault();

	
}


void GLGuidedImageFilter::plot(int w, int h)
{
	vao.bind();
	plotProgram->bind();
	QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
	f->glActiveTexture(GL_TEXTURE0);
	f->glBindTexture(GL_TEXTURE_2D, fboPass[PASS_EQ8]->texture());
	f->glViewport(0, 0, w, h);
	f->glDrawArrays(GL_QUADS, 0, 4);
	plotProgram->release();
	vao.release();
}



QOpenGLShaderProgram* GLGuidedImageFilter::buildProgramPass(int index) const
{
	QStringList shaderFolders;
	shaderFolders.append("shaders/");
	shaderFolders.append("../shaders/");
	shaderFolders.append("../../shaders/");
	shaderFolders.append("../../../shaders/");
	
	QString vertShaderName = "SimpleTexture.vert";
	QString fragShaderName = "MeanFilter.frag";

	switch (index)
	{
		case PASS_MEAN_P: fragShaderName = "MeanFilter.frag"; break;
		case PASS_MEAN_I: fragShaderName = "MeanFilter.frag"; break;
		case PASS_EQ5: fragShaderName = "GuidedImageFilter_Eq5.frag"; break;
		case PASS_EQ6: fragShaderName = "GuidedImageFilter_Eq6.frag"; break;
		case PASS_EQ8: fragShaderName = "GuidedImageFilter_Eq8.frag"; break;
		default:fragShaderName = "SimpleTexture.frag"; break;
	}

	QString vertShaderFile = "";
	QString fragShaderFile = "";

	for (QString folder : shaderFolders)
	{
		if (QFileInfo::exists(folder + vertShaderName) && QFileInfo::exists(folder + fragShaderName))
		{
			vertShaderFile = folder + vertShaderName;
			fragShaderFile = folder + fragShaderName;
			break;
		}
	}

	if (vertShaderFile.isEmpty() || fragShaderFile.isEmpty())
	{
		std::cerr << "<Error> Could not load vertex/fragment shader files: " << "\n\t" 
			<< vertShaderFile.toStdString() << "\n\t"
			<< fragShaderFile.toStdString() << std::endl;

		return GLImageProcessor::buildProgram();
	}

	QOpenGLShaderProgram* prog = new QOpenGLShaderProgram();

	QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, prog);
	QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, prog);

	if (!vshader->compileSourceFile(vertShaderFile))
	{
		QMessageBox::critical(nullptr, tr("GLGuidedImageFilter"),
			QString("Could not load vertex shader file.\n<" + vertShaderFile + ">."),
			QMessageBox::Ok);

		std::cerr << "<Error> Could not load vertex shader file: " << vertShaderFile.toStdString() << std::endl;

		return GLImageProcessor::buildProgram();
	}
	if (!fshader->compileSourceFile(fragShaderFile))
	{
		QMessageBox::critical(nullptr, tr("GLGuidedImageFilter"),
			QString("Could not load vertex shader file.\n<" + fragShaderFile + ">."),
			QMessageBox::Ok);

		std::cerr << "<Error> Could not load fragment shader file: " << fragShaderFile.toStdString() << std::endl;

		return GLImageProcessor::buildProgram();
	}
	
	prog->addShader(vshader);
	prog->addShader(fshader);
	prog->bindAttributeLocation("in_Vertex", 0);
	prog->bindAttributeLocation("in_TexCoord", 1);
	prog->link();
	prog->bind();
	prog->setUniformValue("in_Texture0", 0);
	prog->setUniformValue("in_Texture1", 1);
	prog->setUniformValue("in_Texture2", 2);
	prog->setUniformValue("in_Texture3", 3);
	prog->release();

	return prog;
}



