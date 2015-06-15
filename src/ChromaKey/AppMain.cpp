
#include "MainWindow.h"
#include <QApplication>
#include <QSurfaceFormat>
#include <iostream>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	QSurfaceFormat format;
	format.setDepthBufferSize(24);
	format.setStencilBufferSize(8);
	//format.setVersion(4, 2);
	format.setProfile(QSurfaceFormat::CompatibilityProfile);
	QSurfaceFormat::setDefaultFormat(format);

	MainWindow w;
    w.show();

    return a.exec();
}
