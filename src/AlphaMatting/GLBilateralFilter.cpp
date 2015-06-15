#include "GLBilateralFilter.h"
#include <QMessageBox>
#include <QFileInfo>
#include <iostream>

GLBilateralFilter::GLBilateralFilter(QObject *parent)
	: GLImageProcessor(parent),
	kernelSize(9),
	spatialRadius(1.71f),
	intensityRadius(0.27f)
{
}


GLBilateralFilter::~GLBilateralFilter()
{
}

void GLBilateralFilter::setParameters(int kernel_size, float spatial_radius, float intensity_radius)
{
	kernelSize = kernel_size;
	spatialRadius = spatial_radius;
	intensityRadius = intensity_radius;
}


void GLBilateralFilter::initialize()
{
	GLImageProcessor::initialize();
}





void GLBilateralFilter::render()
{
	vao.bind();
	
	if (!texture.isNull())
		texture->bind();

	program->bind();

	program->setUniformValue("kernelSize", kernelSize);
	program->setUniformValue("intensityRadius", intensityRadius);
	program->setUniformValue("spatialRadius", spatialRadius);

	QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
	f->glDrawArrays(GL_QUADS, 0, 4);
	program->release();

	vao.release();

}




QOpenGLShaderProgram* GLBilateralFilter::buildProgram() const
{
	QStringList shaderFolders;
	shaderFolders.append("shaders/");
	shaderFolders.append("../shaders/");
	shaderFolders.append("../../shaders/");
	shaderFolders.append("../../../shaders/");
	
	QString vertShaderName = "SimpleTexture.vert";
	QString fragShaderName = "BilateralFilter.frag";

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
		QMessageBox::critical(nullptr, tr("GLBilateralFilter"),
			QString("Could not load vertex shader file.\n<" + vertShaderFile + ">."),
			QMessageBox::Ok);

		std::cerr << "<Error> Could not load vertex shader file: " << vertShaderFile.toStdString() << std::endl;

		return GLImageProcessor::buildProgram();
	}
	if (!fshader->compileSourceFile(fragShaderFile))
	{
		QMessageBox::critical(nullptr, tr("GLBilateralFilter"),
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
	prog->setUniformValue("in_Texture", 0);
	prog->release();

	return prog;
}


