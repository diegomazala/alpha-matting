#include "GlobalMatting.h"
#include <fstream>
#include <iostream>


template <typename T>
static inline T sqr(T a)
{
	return a * a;
}


std::vector<QPoint> GlobalMatting::findBoundaryPixels(const QImage &trimap, int a, int b)
{
	std::vector<QPoint> result;

	for (int x = 1; x < trimap.width() - 1; ++x)
		for (int y = 1; y < trimap.height() - 1; ++y)
		{
			int pxl = qRed(trimap.pixel(x, y));	// qRed, qGreen, qBlue return the same value since image is gray scale

			if (pxl == a)
			{
				int pxl_x0 = qRed(trimap.pixel(x - 1, y));
				int pxl_x1 = qRed(trimap.pixel(x + 1, y));
				int pxl_y0 = qRed(trimap.pixel(x, y - 1));
				int pxl_y1 = qRed(trimap.pixel(x, y + 1));

				if (pxl_x0 == b ||
					pxl_x1 == b ||
					pxl_y0 == b ||
					pxl_y1 == b)
				{
					result.push_back(QPoint(x, y));
				}
			}
		}
	return result;
}



// Eq. 2
float GlobalMatting::calculateAlpha(const QRgb &F, const QRgb &B, const QRgb &I)
{
	float result = 0;
	float div = 1e-6f;

	// red channel
	
	float fr = (float)qBlue(F);
	float br = (float)qBlue(B);
	float ir = (float)qBlue(I);

	result += (ir - br) * (fr - br);
	div += (fr - br) * (fr - br);
	
	// green channel
	
	float fg = (float)qGreen(F);
	float bg = (float)qGreen(B);
	float ig = (float)qGreen(I);

	result += (ig - bg) * (fg - bg);
	div += (fg - bg) * (fg - bg);
	
	// blue channel
	
	float fb = (float)qRed(F);
	float bb = (float)qRed(B);
	float ib = (float)qRed(I);

	result += (ib - bb) * (fb - bb);
	div += (fb - bb) * (fb - bb);

	// normalize alpha [0,1]
	return std::min(std::max(result / div, 0.f), 1.f);
}


// Eq. 3
float GlobalMatting::colorCost(const QRgb &F, const QRgb &B, const QRgb &I, float alpha)
{
	float result = 0;
	
	// blue channel
	{
		float f = (float)qRed(F);
		float b = (float)qRed(B);
		float i = (float)qRed(I);

		result += sqr(i - (alpha * f + (1 - alpha) * b));
	}
	// blue channel
	{
		float f = (float)qGreen(F);
		float b = (float)qGreen(B);
		float i = (float)qGreen(I);

		result += sqr(i - (alpha * f + (1 - alpha) * b));
	}
	// blue channel
	{
		float f = (float)qBlue(F);
		float b = (float)qBlue(B);
		float i = (float)qBlue(I);

		result += sqr(i - (alpha * f + (1 - alpha) * b));
	}

	return sqrt(result);
}

// Eq. 4
float GlobalMatting::distCost(const QPoint &p0, const QPoint &p1, float minDist)
{
	int dist = sqr(p0.x() - p1.x()) + sqr(p0.y() - p1.y());
	return sqrt((float)dist) / minDist;
}


float GlobalMatting::colorDist(const QRgb &I0, const QRgb &I1)
{
	int result = 0;

	result += sqr(qRed(I0) - qRed(I1));
	result += sqr(qGreen(I0) - qGreen(I1));
	result += sqr(qBlue(I0) - qBlue(I1));
	
	return sqrt((float)result);
}


float GlobalMatting::nearestDistance(const std::vector<QPoint> &boundary, const QPoint &p)
{
	int minDist2 = INT_MAX;
	for (std::size_t i = 0; i < boundary.size(); ++i)
	{
		int dist2 = sqr(boundary[i].x() - p.x()) + sqr(boundary[i].y() - p.y());
		minDist2 = std::min(minDist2, dist2);
	}

	return sqrt((float)minDist2);
}




void GlobalMatting::calculateAlphaPatchMatch(const QImage &image,
	const QImage &trimap,
	const std::vector<QPoint> &foregroundBoundary,
	const std::vector<QPoint> &backgroundBoundary,
	std::vector<std::vector<Sample> > &samples)
{
	int w = image.width();
	int h = image.height();

	// sample must have the same size of the image
	samples.resize(w, std::vector<Sample>(h));
		
	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			int pxl = qRed(trimap.pixel(x, y));	// qRed, qGreen, qBlue return the same value since image is gray scale
			if (pxl == 128)
			{
				QPoint p(x, y);
				samples[x][y].fi = rand() % foregroundBoundary.size();
				samples[x][y].bj = rand() % backgroundBoundary.size();
				samples[x][y].df = nearestDistance(foregroundBoundary, p);
				samples[x][y].db = nearestDistance(backgroundBoundary, p);
				samples[x][y].cost = FLT_MAX;
			}
		}
	}
	

	// array with coordinates of the image: all pixels
	std::vector<QPoint> coords(w * h);
	for (int y = 0; y < h; ++y)
		for (int x = 0; x < w; ++x)
			coords[x + y * w] = QPoint(x, y);


	for (int iter = 0; iter < 10; ++iter)
	{
		// propagation
		std::random_shuffle(coords.begin(), coords.end());

		for (std::size_t i = 0; i < coords.size(); ++i)
		{
			const QPoint &p = coords[i];

			int x = p.x();
			int y = p.y();

			int pxl = qRed(trimap.pixel(x, y));	// qRed, qGreen, qBlue return the same value since image is gray scale

			if (pxl != 128)
				continue;

			const QRgb &I = image.pixel(x, y);

			Sample &s = samples[x][y];
			
			// mask/kernel 3x3 
			for (int y2 = y - 1; y2 <= y + 1; ++y2)
			{
				for (int x2 = x - 1; x2 <= x + 1; ++x2)
				{
					// ignoring image borders (out of range)
					if (x2 < 0 || x2 >= w || y2 < 0 || y2 >= h)
						continue;

					int pxl2 = qRed(trimap.pixel(x2, y2));	// qRed, qGreen, qBlue return the same value since image is gray scale

					// if this is not a unkown(trimap) pixel, ignore
					if (pxl2 != 128)
						continue;

					Sample &s2 = samples[x2][y2];

					const QPoint &fp = foregroundBoundary[s2.fi];
					const QPoint &bp = backgroundBoundary[s2.bj];
					
					const QRgb F = image.pixel(fp.x(), fp.y());
					const QRgb B = image.pixel(bp.x(), bp.y());

					float alpha = calculateAlpha(F, B, I);
					float cost = colorCost(F, B, I, alpha) + distCost(p, fp, s.df) + distCost(p, bp, s.db);

					if (cost < s.cost)
					{
						s.fi = s2.fi;
						s.bj = s2.bj;
						s.cost = cost;
						s.alpha = alpha;
					}
				}
			}
		}

		// random walk
		int w2 = (int)std::max(foregroundBoundary.size(), backgroundBoundary.size());

		for (int y = 0; y < h; ++y)
		{
			for (int x = 0; x < w; ++x)
			{
				int pxl = qRed(trimap.pixel(x, y));	// qRed, qGreen, qBlue return the same value since image is gray scale

				// if this is not a unkown(trimap) pixel, ignore
				if (pxl != 128)
					continue;

				QPoint p(x, y);

				const QRgb &I = image.pixel(x, y);

				Sample &s = samples[x][y];

				for (int k = 0;; k++)
				{
					float r = w2 * pow(0.5f, k);

					if (r < 1)
						break;

					int di = r * (rand() / (RAND_MAX + 1.f));
					int dj = r * (rand() / (RAND_MAX + 1.f));

					int fi = s.fi + di;
					int bj = s.bj + dj;

					if (fi < 0 || fi >= foregroundBoundary.size() || bj < 0 || bj >= backgroundBoundary.size())
						continue;

					const QPoint &fp = foregroundBoundary[fi];
					const QPoint &bp = backgroundBoundary[bj];

					const QRgb F = image.pixel(fp.x(), fp.y());
					const QRgb B = image.pixel(bp.x(), bp.y());

					float alpha = calculateAlpha(F, B, I);

					float cost = colorCost(F, B, I, alpha) + distCost(p, fp, s.df) + distCost(p, bp, s.db);

					if (cost < s.cost)
					{
						s.fi = fi;
						s.bj = bj;
						s.cost = cost;
						s.alpha = alpha;
					}
				}
			}
		}
	}
}






void GlobalMatting::process()
{
	QMutexLocker locker(&mutex);  // Locks the mutex and unlocks when locker exits the scope

#if 0
	// (optional) exploit the affinity of neighboring pixels to reduce the 
	// size of the unknown region. please refer to the paper
	// 'Shared Sampling for Real-Time Alpha Matting'.
	expansionOfKnownRegions(inputImage, trimapImage, 9);
	trimapPixels(trimapImage, 0, 128, 255);

	QImage qforeground(inputImage);
	qforeground.fill(0);

	outputMaskImage = trimapImage;
	outputMaskImage.fill(0);

	globalMatting(inputImage, trimapImage, qforeground, outputMaskImage);

#if 0
	// TODO: guidedFilter() 
#else
	outputMaskImage = blurMono(outputMaskImage, 3);
#endif

	filterAlphaResultByTrimap(trimapImage, outputMaskImage);

	outputColorImage = inputImage;
	outputColorImage.fill(0);

	alphaBlending(outputMaskImage, inputImage, outputColorImage);
#endif

	QImage qforeground(inputImage);
	qforeground.fill(0);


	emit finished();
}



void GlobalMatting::trimapPixels(QImage &trimap, int min, int mid, int max)
{
	int minmid = mid / 2;
	int maxmid = (max + mid) / 2;

	for (int x = 0; x < trimap.width(); ++x)
	{
		for (int y = 0; y < trimap.height(); ++y)
		{
			int pxl = qRed(trimap.pixel(x, y));	// qRed, qGreen, qBlue return the same value since image is gray scale


			if (pxl < minmid)
				trimap.setPixel(x, y, min);
			else if (pxl > maxmid)
				trimap.setPixel(x, y, max);
			else
				trimap.setPixel(x, y, mid);
		}
	}
}


void GlobalMatting::expansionOfKnownRegions(const QImage img, QImage& trimap, int niter)
{
	if (img.size() != trimap.size())
		std::cerr << "image and trimap mush have same size";

	for (int i = 0; i < niter; ++i)
		expansionOfKnownRegionsHelper(img, trimap, i + 1, niter - i);
}



void GlobalMatting::expansionOfKnownRegionsHelper(const QImage &image, QImage &trimap, int r, float c)
{
	int w = image.width();
	int h = image.height();


	for (int x = 0; x < w; ++x)
	{
		for (int y = 0; y < h; ++y)
		{
			const QRgb tpx = trimap.pixel(x, y);
			if (qRed(tpx) != 128)	// qRed, qGreen, qBlue return the same value since image is gray scale
				continue;

			const QRgb &I = image.pixel(x, y);

			for (int j = y - r; j <= y + r; ++j)
			{
				for (int i = x - r; i <= x + r; ++i)
				{
					if (i < 0 || i >= w || j < 0 || j >= h)
						continue;

					int tpxij = qRed(trimap.pixel(i, j));	// qRed, qGreen, qBlue return the same value since image is gray scale

					if (tpxij != 0 && tpxij != 255)
						continue;

					const QRgb &I2 = image.pixel(i, j);

					float pd = sqrt((float)(sqr(x - i) + sqr(y - j)));
					float cd = colorDist(I, I2);

					if (pd <= r && cd <= c)
					{
						if (tpxij == 0)
							trimap.setPixel(x, y, 1);
						else if (tpxij == 255)
							trimap.setPixel(x, y, 254);
					}

				}
			}
		}
	}


	for (int x = 0; x < trimap.width(); ++x)
	{
		for (int y = 0; y < trimap.height(); ++y)
		{
			int tpx = qRed(trimap.pixel(x, y));	// qRed, qGreen, qBlue return the same value since image is gray scale
			if (tpx == 1)
				trimap.setPixel(x, y, 0);
			else if (tpx == 254)
				trimap.setPixel(x, y, 255);

		}
	}

}


void GlobalMatting::globalMatting(QImage image, QImage trimap, QImage& foreground, QImage& alpha)
{
	if (image.size() != trimap.size())
		std::cerr << "image and trimap mush have same size" << std::endl;

	globalMattingHelper(image, trimap, foreground, alpha);
		
}




void GlobalMatting::globalMattingHelper(QImage image, QImage trimap, QImage &foreground, QImage &alpha)
{
	std::vector<QPoint> foregroundBoundary = findBoundaryPixels(trimap, 255, 128);
	std::vector<QPoint> backgroundBoundary = findBoundaryPixels(trimap, 0, 128);

	QImage bound(trimap);

	bound.fill(0);
	for (auto p : foregroundBoundary)
		bound.setPixel(p.x(), p.y(), 255);

	bound.fill(0);
	for (auto p : backgroundBoundary)
		bound.setPixel(p.x(), p.y(), 255);

	// Estimating samples. Based on unkown pixels, i.e. boundary pixels, collect global samples
	int n = (int)(foregroundBoundary.size() + backgroundBoundary.size());
	for (int i = 0; i < n; ++i)
	{
		int x = rand() % trimap.width();
		int y = rand() % trimap.height();

		int pxl = qRed(trimap.pixel(x, y));	// qRed, qGreen, qBlue return the same value since image is gray scale

		if (pxl == 0)
			backgroundBoundary.push_back(QPoint(x, y));
		else if (pxl == 255)
			foregroundBoundary.push_back(QPoint(x, y));
	}

	bound.fill(0);
	for (auto p : foregroundBoundary)
		bound.setPixel(p.x(), p.y(), 255);

	bound.fill(0);
	for (auto p : backgroundBoundary)
		bound.setPixel(p.x(), p.y(), 255);

	// Order boundary pixels based on their intensity
	std::sort(foregroundBoundary.begin(), foregroundBoundary.end(), QIntensityComp(image));
	std::sort(backgroundBoundary.begin(), backgroundBoundary.end(), QIntensityComp(image));

	// 
	std::vector<std::vector<Sample> > samples;
	calculateAlphaPatchMatch(image, trimap, foregroundBoundary, backgroundBoundary, samples);

	for (int y = 0; y < alpha.height(); ++y)
	{
		for (int x = 0; x < alpha.width(); ++x)
		{
			int pxl = qRed(trimap.pixel(x, y));	// qRed, qGreen, qBlue return the same value since image is gray scale
			switch (pxl)
			{
			case 0:
			{
				alpha.setPixel(x, y, 0);
				foreground.setPixel(x, y, 0);
				break;
			}
			case 128:
			{
				uint a = 255 * samples[x][y].alpha;
				alpha.setPixel(x, y, a);
				QPoint p = foregroundBoundary[samples[x][y].fi];
				foreground.setPixel(x, y, image.pixel(p.x(), p.y()));
				break;
			}
			case 255:
			{
				alpha.setPixel(x, y, 255);
				foreground.setPixel(x, y, image.pixel(x, y));
				break;
			}
			}
		}
	}
}


void GlobalMatting::filterAlphaResultByTrimap(const QImage& trimap, QImage& alpha)
{
	for (int x = 0; x < trimap.width(); ++x)
	{
		for (int y = 0; y < trimap.height(); ++y)
		{
			int pxl = qRed(trimap.pixel(x, y));	// qRed, qGreen, qBlue return the same value since image is gray scale

			if (pxl == 0)
				alpha.setPixel(x, y, 0);
			else if (pxl == 255)
				alpha.setPixel(x, y, 255);
		}
	}
}


void GlobalMatting::alphaBlending(const QImage& alpha, const QImage& foreground, QImage& result)
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





QImage GlobalMatting::blur(const QImage& image, const QRect& rect, int radius, bool alphaOnly)
{
	int tab[] = { 14, 10, 8, 6, 5, 5, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2 };
	int alpha = (radius < 1) ? 16 : (radius > 17) ? 1 : tab[radius - 1];

	QImage result = image.convertToFormat(QImage::Format::Format_ARGB32_Premultiplied);
	int r1 = rect.top();
	int r2 = rect.bottom();
	int c1 = rect.left();
	int c2 = rect.right();

	int bpl = result.bytesPerLine();
	int rgba[4];
	unsigned char* p;

	int i1 = 0;
	int i2 = 3;

	if (alphaOnly)
		i1 = i2 = (QSysInfo::ByteOrder == QSysInfo::BigEndian ? 0 : 3);


	for (int col = c1; col <= c2; col++) {
		p = result.scanLine(r1) + col * 4;
		for (int i = i1; i <= i2; i++)
			rgba[i] = p[i] << 4;

		p += bpl;
		for (int j = r1; j < r2; j++, p += bpl)
			for (int i = i1; i <= i2; i++)
				p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
	}

	for (int row = r1; row <= r2; row++) {
		p = result.scanLine(row) + c1 * 4;
		for (int i = i1; i <= i2; i++)
			rgba[i] = p[i] << 4;

		p += 4;
		for (int j = c1; j < c2; j++, p += 4)
			for (int i = i1; i <= i2; i++)
				p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
	}

	for (int col = c1; col <= c2; col++) {
		p = result.scanLine(r2) + col * 4;
		for (int i = i1; i <= i2; i++)
			rgba[i] = p[i] << 4;

		p -= bpl;
		for (int j = r1; j < r2; j++, p -= bpl)
			for (int i = i1; i <= i2; i++)
				p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
	}

	for (int row = r1; row <= r2; row++) {
		p = result.scanLine(row) + c2 * 4;
		for (int i = i1; i <= i2; i++)
			rgba[i] = p[i] << 4;

		p -= 4;
		for (int j = c1; j < c2; j++, p -= 4)
			for (int i = i1; i <= i2; i++)
				p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
	}

	return result;
}


uint sum(const QImage img, int r1, int r2, int c1, int c2)
{
	int s = 0;
	for (int i = r1; i < r2; ++i)
		for (int j = c1; j < c2; ++j)
			s += qRed(img.pixel(i, j));
	return s;
}

QImage GlobalMatting::blurMono(const QImage& img, int radius)
{
	QImage tmp(img);
	QImage out(img);

	const int mid = radius / 2;
	// horizontal
	for (int y = 0; y < img.height(); ++y)
	{
		for (int x = 0; x < img.width(); ++x)
		{
			uint res = 0;
			for (int dx = -mid; dx <= mid; ++dx)
			{
				int kx = qBound(0, x + dx, img.width() - 1);
				res += qRed(img.pixel(kx, y));
			}
			res = int(float(res) / float(radius));
			tmp.setPixel(x, y, res);
		}
	}

	// vertical
	for (int x = 0; x < img.width(); ++x)
	{
		for (int y = 0; y < img.height(); ++y)
		{
			uint res = 0;
			for (int dy = -mid; dy <= mid; ++dy)
			{
				int ky = qBound(0, y + dy, img.height() - 1);
				res += qRed(tmp.pixel(x, ky));
			}
			res = int(float(res) / float(radius));
			out.setPixel(x, y, res);
		}
	}

	return out;
}

