
#include <iostream>
#include <QImage>


static void alphaBlending(const QImage& background, const QImage& alpha, const QImage& foreground, QImage& result)
{
	for (int x = 0; x < alpha.width(); ++x)
	{
		for (int y = 0; y < alpha.height(); ++y)
		{
			int a = qRed(alpha.pixel(x, y));	// qRed, qGreen, qBlue return the same value since image is gray scale

			const QRgb &fg = foreground.pixel(x, y);
			int fg_r = qRed(fg);
			int fg_g = qGreen(fg);
			int fg_b = qBlue(fg);

			/*int r = (255 - a) * 255 + a * fg_r;
			int g = (255 - a) * 255 + a * fg_g;
			int b = (255 - a) * 255 + a * fg_b;*/

			int r = (a * fg_r) / 255;
			int g = (a * fg_g) / 255;
			int b = (a * fg_b) / 255;

			result.setPixel(x, y, qRgb(r, g, b));
			//result.setPixel(x, y, fg);

			/*if (pxl == 0)
			result.setPixel(x, y, 0);
			else if (pxl == 255)
			result.setPixel(x, y, 255);*/
		}
	}
}

static void alphaBlending(const QImage& foreground, const QImage& alpha, QImage& result)
{
	for (int x = 0; x < alpha.width(); ++x)
	{
		for (int y = 0; y < alpha.height(); ++y)
		{
			int a = qRed(alpha.pixel(x, y));	// qRed, qGreen, qBlue return the same value since image is gray scale

			const QRgb &fg = foreground.pixel(x, y);
			int fg_r = qRed(fg);
			int fg_g = qGreen(fg);
			int fg_b = qBlue(fg);


			int r = (a * fg_r) / 255;
			int g = (a * fg_g) / 255;
			int b = (a * fg_b) / 255;

			result.setPixel(x, y, qRgb(r, g, b));
		}
	}
}

int main(int argc, char* argv[])
{
	std::string img_file_1 = "img_color.png";
	std::string img_file_2 = "img_alpha_mask.png";
	std::string result_file = "composite_result.png";

	if (argc > 2)
	{
		img_file_1 = argv[1];
		img_file_2 = argv[2];
		
		if (argc > 3)
			result_file = argv[3];
	}
	else
	{
		std::cerr << "Usage: app.exe <img_color.png> <img_alpha_mask.png> <composite_result.png>" << std::endl;
	}


	QImage img_color(img_file_1.c_str());
	img_color = img_color.convertToFormat(QImage::Format::Format_RGB888);
	QImage img_alpha(img_file_2.c_str());
	img_alpha = img_alpha.convertToFormat(QImage::Format::Format_Indexed8);

	QImage img_blend(img_color);

	if (img_color.size() != img_alpha.size())
	{
		std::cerr << "<Error> The images have different sizes" << std::endl;
		return EXIT_FAILURE;
	}

	alphaBlending(img_color, img_alpha, img_blend);
	
	img_blend.save(result_file.c_str());

	return EXIT_SUCCESS;
}
