#ifndef _GLOBAL_MATTING_H_
#define _GLOBAL_MATTING_H_


#include <QImage>
#include <QThread>
#include <QTimer>



struct Sample
{
	int fi, bj;
	float df, db;
	float cost, alpha;
};


// for sorting the boundary pixels according to intensity
struct QIntensityComp
{
	QIntensityComp(const QImage &image) : image(image)
	{

	}

	bool operator()(const QPoint &p0, const QPoint &p1) const
	{
		const QRgb &c0 = image.pixel(p0.x(), p0.y());
		const QRgb &c1 = image.pixel(p1.x(), p1.y());

		int c0_sum = qRed(c0) + qGreen(c0) + qBlue(c0);
		int c1_sum = qRed(c1) + qGreen(c1) + qBlue(c1);

		return c0_sum < c1_sum;
	}

	const QImage &image;
};



class GlobalMatting : public QObject
{
	Q_OBJECT
		
public:

	explicit GlobalMatting(){}
	~GlobalMatting(){}

	const QImage& getInputImage() const { return inputImage; };
	const QImage& getTrimapImage() const{ return trimapImage; };
	const QImage& getOutputMaskImage() const{ return outputMaskImage; };
	const QImage& getOutputColorImage() const{ return outputColorImage; };

public slots:

	void loadInputImage(const QString& imagefile)	
	{ 
		inputImage.load(imagefile);
	};

	void loadTrimapImage(const QString& imagefile)
	{
		trimapImage.load(imagefile);
		trimapImage = trimapImage.convertToFormat(QImage::Format_Indexed8, Qt::MonoOnly);
	};
		

	void setInputImage(const QImage& image)
	{
		inputImage = image;
	};

	void setTrimapImage(const QImage& image)
	{
		trimapImage = image;
	};


	void process();


private:

	static std::vector<QPoint> findBoundaryPixels(const QImage &trimap, int a, int b);
	static float calculateAlpha(const QRgb &F, const QRgb &B, const QRgb &I);
	static float colorCost(const QRgb &F, const QRgb &B, const QRgb &I, float alpha);
	static float distCost(const QPoint &p0, const QPoint &p1, float minDist);
	static float colorDist(const QRgb &I0, const QRgb &I1);
	static float nearestDistance(const std::vector<QPoint> &boundary, const QPoint &p);

	static void calculateAlphaPatchMatch(const QImage &image,
										const QImage &trimap,
										const std::vector<QPoint> &foregroundBoundary,
										const std::vector<QPoint> &backgroundBoundary,
										std::vector<std::vector<Sample> > &samples);
	
	static void trimapPixels(QImage &trimap, int min, int mid, int max);
	static void expansionOfKnownRegionsHelper(const QImage &image, QImage &trimap, int r, float c);
	static void expansionOfKnownRegions(const QImage img, QImage& trimap, int niter = 9);
	static void globalMatting(QImage image, QImage trimap, QImage& foreground, QImage& alpha);
	static void globalMattingHelper(QImage image, QImage trimap, QImage &foreground, QImage &alpha);
	static void filterAlphaResultByTrimap(const QImage& trimap, QImage& alpha);
	static void alphaBlending(const QImage& alpha, const QImage& foreground, QImage& result);

	static QImage blur(const QImage& image, const QRect& rect, int radius, bool alphaOnly = false);
	static QImage blurMono(const QImage& img, int radius);

	//static QImage convertToGrayScale(const QImage& img_color);
	
signals:
	void finished();
	void error(QString err);



private: // attibutes
	QImage inputImage;
	QImage trimapImage;
	QImage outputMaskImage;
	QImage outputColorImage;
	QMutex mutex;
};



#endif	// _GLOBAL_MATTING_H_
