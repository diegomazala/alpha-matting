#include "GlobalMattingWidget.h"
#include "ui_GlobalMattingWidget.h"
#include <QFileInfo>
#include <iostream>

GlobalMattingWidget::GlobalMattingWidget(QWidget *parent) :
			QWidget(parent),
			ui(new Ui::GlobalMattingWidget)
{
	ui->setupUi(this);
}

GlobalMattingWidget::~GlobalMattingWidget()
{
	
}


void GlobalMattingWidget::loadInputImage(const QString& imagefile)
{
	matting.loadInputImage(imagefile);
	ui->inputLabel->setPixmap(QPixmap::fromImage(matting.getInputImage()));
}


void GlobalMattingWidget::loadTrimapImage(const QString& imagefile)
{
	matting.loadTrimapImage(imagefile);
	ui->trimapLabel->setPixmap(QPixmap::fromImage(matting.getInputImage()));
}


void GlobalMattingWidget::setInputImage(const QImage& image)
{
	matting.setInputImage(image);
	ui->inputLabel->setPixmap(QPixmap::fromImage(image));
}


void GlobalMattingWidget::setTrimapImage(const QImage& trimap)
{
	matting.setTrimapImage(trimap);
	ui->trimapLabel->setPixmap(QPixmap::fromImage(trimap));
}


void GlobalMattingWidget::setOutputMaskImage(const QImage& output)
{
	ui->outputMaskLabel->setPixmap(QPixmap::fromImage(output));
}


void GlobalMattingWidget::errorString(QString err)
{
	std::cerr << err.toStdString() << std::endl;
}

void GlobalMattingWidget::done()
{
	setCursor(Qt::ArrowCursor);
	ui->outputMaskLabel->setPixmap(QPixmap::fromImage(matting.getOutputMaskImage()));
	ui->outputColorLabel->setPixmap(QPixmap::fromImage(matting.getOutputColorImage()));

	QFileInfo f(resultFileName);
	matting.getOutputMaskImage().save(f.path() + '/' + f.baseName() + "_alpha.png");
	matting.getOutputColorImage().save(f.path() + '/' + f.baseName() + "_color.png");
}


void GlobalMattingWidget::execute()
{
	setCursor(Qt::WaitCursor);

	QThread* thread = new QThread;
	matting.moveToThread(thread);
	connect(&matting, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
	connect(thread, SIGNAL(started()), &matting, SLOT(process()));
	connect(&matting, SIGNAL(finished()), thread, SLOT(quit()));
	//connect(&matting, SIGNAL(finished()), &matting, SLOT(deleteLater()));
	connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
	connect(thread, SIGNAL(finished()), this, SLOT(done()));
	thread->start();
}