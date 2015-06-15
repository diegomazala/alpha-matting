#ifndef _GLOBAL_MATTING_WIDGET_H_
#define _GLOBAL_MATTING_WIDGET_H_


#include <QWidget>
#include "GlobalMatting.h"

namespace Ui
{
	class GlobalMattingWidget;
}


class GlobalMattingWidget : public QWidget
{
	Q_OBJECT

public:
	
	explicit GlobalMattingWidget(QWidget *parent = 0);
	~GlobalMattingWidget();

	void execute();

	void setResultFileName(QString result_file_name) { resultFileName = result_file_name; };

public slots:
	void loadInputImage(const QString& imagefile);
	void loadTrimapImage(const QString& imagefile);

	void setInputImage(const QImage& image);
	void setTrimapImage(const QImage& trimap);
	void setOutputMaskImage(const QImage& output);

	void errorString(QString);
	void done();

private:

	Ui::GlobalMattingWidget *ui;
	GlobalMatting matting;
	QString resultFileName;
};

#endif // _GLOBAL_MATTING_WIDGET_H_