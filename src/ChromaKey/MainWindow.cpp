

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "AboutDialog.h"
#include <QKeyEvent>
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent) : 
			QMainWindow(parent),
			ui(new Ui::MainWindow),
			currentFileName(QString())
{
	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	delete ui;
}


void MainWindow::fileOpen()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "../../../data", tr("Images (*.png *.bmp *.jpg *.tif)"));

	if (!fileName.isEmpty())
	{
		currentFileName = fileName;
		// load image
		ui->glImageWidget->setImage(QImage(fileName));
	}
}


void MainWindow::fileSave()
{
	if (!currentFileName.isEmpty())
	{
		
	}
	else
	{
		fileSaveAs();
	}
}


void MainWindow::fileSaveAs()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Images (*.png *.bmp *.jpg *.tiff)"));
	if (!fileName.isEmpty())
	{
		currentFileName = fileName;
		fileSave();
	}
}


void MainWindow::aboutDialogShow()
{
	aboutDialog = new AboutDialog(this);
	aboutDialog->show();
}


void MainWindow::updateColor()
{
	QPalette minColorPalette;
	minColorPalette.setColor(QPalette::Window, ui->glImageWidget->getMinColorRGB());
	ui->minColorLabel->setAutoFillBackground(true);
	ui->minColorLabel->setPalette(minColorPalette);
	
	QPalette maxColorPalette;
	maxColorPalette.setColor(QPalette::Window, ui->glImageWidget->getMaxColorRGB());
	ui->maxColorLabel->setAutoFillBackground(true);
	ui->maxColorLabel->setPalette(maxColorPalette);
}
