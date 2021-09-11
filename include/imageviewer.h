#ifndef IMAGEVIEWER_H_INCLUDED
#define IMAGEVIEWER_H_INCLUDED

#include <QWidget>
#include <QPaintEvent>
#include <QString>
#include <QPixmap>

#include "viewerif.h"

class ImageViewer: public ViewerInterface
{
		Q_OBJECT

		QPixmap rawPixmap;
	public:
		ImageViewer(QWidget* parent=nullptr);
		virtual ~ImageViewer() {}

	protected:
		virtual bool load(const QString& srcImageFile);
		virtual void paintEvent(QPaintEvent *event);
};

#endif
