#ifndef _ALPHA_MATTING_MAIN_WINDOW_H_
#define _ALPHA_MATTING_MAIN_WINDOW_H_


#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
QT_END_NAMESPACE

namespace Ui 
{
	class AlphaMattingMainWindow;
}


class AlphaMattingMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	
	explicit AlphaMattingMainWindow(QWidget *parent = 0);
	~AlphaMattingMainWindow();

public slots:
	void fileSave();
	void aboutDialogShow();


private slots:

private:

	Ui::AlphaMattingMainWindow *ui;
};

#endif // _ALPHA_MATTING_MAIN_WINDOW_H_
