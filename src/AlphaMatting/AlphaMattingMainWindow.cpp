#include "AlphaMattingMainWindow.h"
#include "ui_AlphaMattingMainWindow.h"
#include <QFileDialog>
#include <QMessageBox>


AlphaMattingMainWindow::AlphaMattingMainWindow(QWidget *parent) :
			QMainWindow(parent),
			ui(new Ui::AlphaMattingMainWindow)
{
	ui->setupUi(this);
}


AlphaMattingMainWindow::~AlphaMattingMainWindow()
{
	delete ui;
}



void AlphaMattingMainWindow::fileSave()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Images (*.png *.bmp *.jpg *.tiff)"));
	if (!fileName.isEmpty())
	{
		ui->glImageWidget->grabFramebuffer().save(fileName);
	}
}



void AlphaMattingMainWindow::aboutDialogShow()
{
	QString message
		("<p>Alpha Matting algorithm using Qt and Opengl" \
		"<p><p>" \
		"<br>   [1] K. He, C. Rhemann, C. Rother, X. Tang, J. Sun, A Global Sampling Method for Alpha Matting, CVPR, 2011. <br>" \
		"<br>   [2] C. Tomasi and R. Manduchi, Bilateral Filtering for Gray and Color Images, Proc.IEEE Intel Computer Vision Conference, 1998. <br>" \
		"<br>   [3] K.He, J.Sun, and X.Tang, Guided Image Filtering,  Proc. European Conf.Computer Vision, pp. 1 - 14, 2010. <br>" \
		"<p><p><p>" \
		"<p>Developed by: Diego Mazala, June-2015" \
		"<p>");

	QMessageBox::about(this, tr("Alpha Matting"), message);

}

