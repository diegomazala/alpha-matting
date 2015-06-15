#include "QPixmapLabel.h"

QPixmapLabel::QPixmapLabel(QWidget *parent) : QLabel(parent)
{
	this->setMinimumSize(1, 1);
}


void QPixmapLabel::setPixmap(const QPixmap & p)
{
	pix = p;
	QLabel::setPixmap(p);
}


int QPixmapLabel::heightForWidth(int width) const
{
	return ((qreal)pix.height()*width) / pix.width();
}


QSize QPixmapLabel::sizeHint() const
{
	int w = this->width();
	return QSize(w, heightForWidth(w));
}


void QPixmapLabel::resizeEvent(QResizeEvent * e)
{
	if (!pix.isNull())
		QLabel::setPixmap(pix.scaled(this->size(), Qt::KeepAspectRatio, Qt::FastTransformation));
}