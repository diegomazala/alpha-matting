
#include "GlobalMattingWidget.h"
#include <QApplication>
#include <iostream>


QImage convertToGrayScale(const QImage& img_color)
{
	QVector<QRgb>  colorTable;
	for (int i = 0; i < 256; ++i)
		colorTable.push_back(qRgb(i, i, i));

	QImage img_gray(img_color.size(), QImage::Format::Format_Indexed8);
	img_gray.setColorTable(colorTable);

	for (int x = 0; x < img_gray.width(); ++x)
		for (int y = 0; y < img_gray.height(); ++y)
			img_gray.setPixel(x, y, qGray(img_color.pixel(x, y)));
	
	return img_gray;
}


int main(int argc, char* argv[])
{
	std::string imageFile = "image.png";
	std::string trimapFile = "trimap.png";
	std::string resultFile = "result.png";

	if (argc > 1)
	{
		imageFile = argv[1];
		if (argc > 2)
			trimapFile = argv[2];
		if (argc > 3)
			resultFile = argv[3];
	}
	else
	{
		std::cerr << "Usage: app.exe <image.png> <trimap.png> <result.png>" << std::endl;
	}


	QApplication a(argc, argv);
	GlobalMattingWidget w;

	QImage qimage(imageFile.c_str());
	QImage qtrimap(convertToGrayScale(QImage(trimapFile.c_str())));

	w.setInputImage(qimage);
	w.setTrimapImage(qtrimap);
	w.setResultFileName(resultFile.c_str());

	w.show();

	w.execute();

	//a.exit();
	return a.exec();

	//return EXIT_SUCCESS;
}
