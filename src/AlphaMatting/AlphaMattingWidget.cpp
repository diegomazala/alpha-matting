
#include "AlphaMattingWidget.h"
#include "ui_AlphaMattingWidget.h"
#include <QFileDialog>
#include <iostream>

AlphaMattingWidget::AlphaMattingWidget(QWidget *parent) : 
		QWidget(parent), 
		ui(new Ui::AlphaMattingWidget),
		inputImage(1, 1, QImage::Format_RGB888),
		trimapImage(1, 1, QImage::Format_Indexed8),
		outputAlphaImage(1, 1, QImage::Format_Indexed8),
		outputColorImage(1, 1, QImage::Format_RGB888),
		matting(nullptr)
{
	ui->setupUi(this);

	inputImage.fill(0);
	trimapImage.fill(0);
	outputAlphaImage.fill(0);
	outputColorImage.fill(0);

	ui->filteringGroupBox->setChecked(false);
}

AlphaMattingWidget::~AlphaMattingWidget()
{
}


void AlphaMattingWidget::openInputImage()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "./", tr("Images (*.png *.bmp *.jpg *.tif)"));
	if (!fileName.isEmpty())
		inputImage.load(fileName);		// load image

	ui->inputImageRadioButton->setChecked(true);
	onInputRadioButtonToggled(true);
}

void AlphaMattingWidget::openTrimapImage()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "./", tr("Images (*.png *.bmp *.jpg *.tif)"));
	if (!fileName.isEmpty())
	{
		trimapImage.load(fileName);	// load image
	}

	ui->trimapImageRadioButton->setChecked(true);
	onTrimapRadioButtonToggled(true);
}


void AlphaMattingWidget::onInputRadioButtonToggled(bool toggled)
{
	if (toggled)
	{
		emit showImage(inputImage);
		onBilateralFilterUpdate();
		onGuidedFilterUpdate();
	}
	
}

void AlphaMattingWidget::onTrimapRadioButtonToggled(bool toggled)
{
	if (toggled)
	{
		emit showImage(trimapImage);
		onBilateralFilterUpdate();
		onGuidedFilterUpdate();
	}
}

void AlphaMattingWidget::onOutputAlphaRadioButtonToggled(bool toggled)
{
	if (toggled)
	{
		emit showImage(outputAlphaImage);
		onBilateralFilterUpdate();
		onGuidedFilterUpdate();
	}
}

void AlphaMattingWidget::onOutputColorRadioButtonToggled(bool toggled)
{
	if (toggled)
	{
		emit showImage(outputColorImage);
		onBilateralFilterUpdate();
		onGuidedFilterUpdate();
	}
}


void AlphaMattingWidget::errorString(QString err)
{
	std::cerr << err.toStdString() << std::endl;
}

void AlphaMattingWidget::done()
{
	setCursor(Qt::ArrowCursor);

	outputAlphaImage = matting.getOutputMaskImage();
	outputColorImage = matting.getOutputColorImage();
	ui->outputAlphaRadioButton->setChecked(true);
	onOutputAlphaRadioButtonToggled(true);

	onBilateralFilterUpdate();
	onGuidedFilterUpdate();
}


void AlphaMattingWidget::onRunButtonClicked()
{
//	inputImage.load("G:/Projects/imgproc/data/image.png");
//	trimapImage.load("G:/Projects/imgproc/data/trimap.png");
	
	matting.setInputImage(inputImage);
	matting.setTrimapImage(trimapImage);

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

void AlphaMattingWidget::onSaveButtonClicked()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "../../../data", tr("Image (*.png)"));
	if (!fileName.isEmpty())
	{
		QFileInfo f(fileName);
		outputAlphaImage.save(f.path() + '/' + f.baseName() + "_alpha.png");
		outputColorImage.save(f.path() + '/' + f.baseName() + "_color.png");
	}
}

void AlphaMattingWidget::onFilteringToggled(bool toggled)
{
	if (ui->filteringGroupBox->isChecked())					// Filtering is on
	{
		if (ui->bilateralGroupBox->isChecked())				// Filter::Bilateral is on
			emit filterChanged(int(Filter::Bilateral));		// Emit signal
		else if (ui->guidedGroupBox->isChecked())			// Filter::Guided is on
			emit filterChanged(int(Filter::Guided));		// Emit signal
		//else												// None filter selected. Set one of those and emit signal
		//	ui->guidedGroupBox->setChecked(true);			// Turn on Filter::Guided
	}
	else													// Filtering is off
	{
		emit filterChanged(int(Filter::None));
	}

	onBilateralFilterUpdate();
	onGuidedFilterUpdate();
}

void AlphaMattingWidget::onBilateralFilterToggled(bool toggled)
{
	if (toggled)
	{
		ui->guidedGroupBox->setChecked(false);				// Turn off Filter::Guided
		emit filterChanged(int(Filter::Bilateral));			// Emit signal
		onBilateralFilterUpdate();
	}
	else
	{
		ui->guidedGroupBox->setChecked(true);				// Turn on Filter::Guided
	}
}

void AlphaMattingWidget::onGuidedFilterToggled(bool toggled)
{
	if (toggled)
	{
		ui->bilateralGroupBox->setChecked(false);			// Turn off Filter::Guided
		emit filterChanged(int(Filter::Guided));			// Emit signal
		onGuidedFilterUpdate();
	}
	else
	{
		ui->bilateralGroupBox->setChecked(true);			// Turn on Filter::Guided
	}
}


void AlphaMattingWidget::onBilateralFilterUpdate()
{
	emit bilateralFilterChanged(
		ui->bilateralKernelSpinBox->value(),
		ui->bilateralSpatialSpinBox->value(),
		ui->bilateralIntensitySpinBox->value());
}

void AlphaMattingWidget::onGuidedFilterUpdate()
{
	emit guidedFilterChanged(
		ui->guidedKernelSpinBox->value(),
		ui->guidedRegularizationSpinBox->value(),
		ui->guidedImageGroupBox->isChecked(),
		ui->guideOutputAlphaRadioButton->isChecked() ? matting.getOutputMaskImage() : matting.getInputImage());
}