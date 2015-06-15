#ifndef _Q_PIXMAP_LABEL_H_
#define _Q_PIXMAP_LABEL_H_

#include <QLabel>
#include <QPixmap>
#include <QResizeEvent>

class QPixmapLabel : public QLabel
{
	Q_OBJECT

public:
	explicit QPixmapLabel(QWidget *parent = 0);
	virtual int heightForWidth(int width) const;
	virtual QSize sizeHint() const;


public slots :
	void setPixmap(const QPixmap &);
	void resizeEvent(QResizeEvent *);

private:
	QPixmap pix;
};

#endif // _Q_PIXMAP_LABEL_H_