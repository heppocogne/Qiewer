#include "imageviewer.h"

#include <QImageReader>
#include <QPainter>

#include "configure.h"


ImageViewer::ImageViewer(QWidget *parent)
	:ViewerInterface(parent)
{

}


bool ImageViewer::load(const QString& srcImageFile)
{
	QImageReader reader(srcImageFile);
	if(reader.canRead()) {
		rawPixmap=QPixmap::fromImage(reader.read());
		rawSize=rawPixmap.size();
		
		return true;
	} else {
		unknownFormatErrorDialog();
		
		return false;
	}
}


void ImageViewer::paintEvent(QPaintEvent *event)
{
	if(event==nullptr) {}	//warning avoidance

	QPainter painter(viewport());
	painter.setRenderHint(QPainter::Antialiasing, configure.raster_antialiasing);

	painter.drawPixmap(pixmapRect, rawPixmap);
}
