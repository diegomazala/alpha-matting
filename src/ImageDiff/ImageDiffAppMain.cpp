
#include <iostream>
#include <QImage>


int main(int argc, char* argv[])
{

	std::string img_file_1 = "img1.png";
	std::string img_file_2 = "img2.png";
	std::string result_file = "diff.png";

	if (argc > 2)
	{
		img_file_1 = argv[1];
		img_file_2 = argv[2];
		
		if (argc > 3)
			result_file = argv[3];
	}
	else
	{
		std::cerr << "Usage: app.exe <img_file_1.png> <img_file_2.png> <diff_result.png>" << std::endl;
	}


	QImage img1(img_file_1.c_str());
	img1 = img1.convertToFormat(QImage::Format::Format_Indexed8);
	QImage img2(img_file_2.c_str());
	img2 = img2.convertToFormat(QImage::Format::Format_Indexed8);

	QImage img_diff(img1);

	if (img1.size() != img2.size())
	{
		std::cerr << "<Error> The images have different sizes" << std::endl;
		return EXIT_FAILURE;
	}

	
	for (int x = 0; x < img1.width(); x++)
	{
		for (int y = 0; y < img1.height(); y++)
		{
			int px1 = qRed(img1.pixel(x, y));	// qRed, qGreen, qBlue return the same value since image is gray scale
			int px2 = qRed(img2.pixel(x, y));	// qRed, qGreen, qBlue return the same value since image is gray scale
			int diff = abs(px1 - px2);

			if (px1 != px2)
				img_diff.setPixel(x, y, diff);
			else
				img_diff.setPixel(x, y, 0);
		}
	}
	
	img_diff.save(result_file.c_str());

	return EXIT_SUCCESS;
}
