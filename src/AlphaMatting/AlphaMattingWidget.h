#ifndef _ALPHA_MATTING_WIDGET_H_
#define _ALPHA_MATTING_WIDGET_H_

#include <QWidget>
#include "GlobalMatting.h"

namespace Ui
{
	class AlphaMattingWidget;
}


class AlphaMattingWidget : public QWidget
{
	Q_OBJECT

public:
	AlphaMattingWidget(QWidget *parent = 0);
	~AlphaMattingWidget();

	enum Filter
	{
		None,
		Bilateral,
		Guided
	};

	QSize minimumSizeHint() const Q_DECL_OVERRIDE{ return sizeHint(); };
	QSize sizeHint() const Q_DECL_OVERRIDE{ return QSize(266, 500); };

protected slots:
	void openInputImage();
	void openTrimapImage();
	void onInputRadioButtonToggled(bool);
	void onTrimapRadioButtonToggled(bool);
	void onOutputAlphaRadioButtonToggled(bool);
	void onOutputColorRadioButtonToggled(bool);
	void onRunButtonClicked();
	void onSaveButtonClicked();
	void onFilteringToggled(bool);
	void onBilateralFilterToggled(bool);
	void onGuidedFilterToggled(bool);
	void onBilateralFilterUpdate();
	void onGuidedFilterUpdate();
	void errorString(QString);
	void done();

protected:

signals:
	void showImage(QImage);
	void filterChanged(int);
	void bilateralFilterChanged(int, float, float);
	void guidedFilterChanged(int, float, bool, QImage);

private:

	enum
	{
		INPUT,
		TRIMAP,
		OUTPUT
	};


	Ui::AlphaMattingWidget *ui;

	QImage inputImage;
	QImage trimapImage;
	QImage outputAlphaImage;
	QImage outputColorImage;

	GlobalMatting matting;
};


#endif // _ALPHA_MATTING_WIDGET_H_
