#include "imageviewer.h"
#include "mainwindow.h"
#include <Qt>
#include <QString>
#include <QPixmap>
#include <QScreen>
#include <QGuiApplication>
//#include "logger.h"
#include <cmath>
#include <QGraphicsScene>


const double ImageViewer::virtualScaleMax=10.0;
const double ImageViewer::virtualScaleMin=0.1;


ImageViewer::ImageViewer(QWidget *parent=nullptr)
	:QGraphicsView(parent),
	 leftClick(false),
	 baseScale(1.0),
	 virtualLogScale(0.0)
{
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setCacheMode(QGraphicsView::CacheBackground);
	setMouseTracking(true);
}


void ImageViewer::setImage(const QImage& image)
{
	rawPixmap=QPixmap::fromImage(image);

	baseScale=1.0;
	virtualLogScale=0.0;

	positionMapping(QPointF(rawPixmap.width(), rawPixmap.height())/2, QPointF(width(), height())/2);
}


void ImageViewer::positionMapping(const QPointF& pixmapPos, const QPointF& screenPos)
{
	const auto delta=screenPos-mapFromItem(pixmapPos);
	updatePixmapRect(pixmapRect.x()+delta.x(), pixmapRect.y()+delta.y());
}


QSize ImageViewer::sizeHint(void)const
{
	return QSize(pixmapRect.width(), pixmapRect.height());
}


double ImageViewer::virtualScale(void)const
{
	return std::pow(10,virtualLogScale);
}

void ImageViewer::setVirtualScale(double v_scale)
{
	if(virtualScaleMax<v_scale) {
		v_scale=virtualScaleMax;
	} else if(v_scale<virtualScaleMin) {
		v_scale=virtualScaleMin;
	}
	virtualLogScale=log10(v_scale);
}

void ImageViewer::setVirtualLogScale(double vlog_scale)
{
	const double vlog_max=std::log10(virtualScaleMax);
	const double vlog_min=std::log10(virtualScaleMin);
	if(vlog_max<vlog_scale) {
		vlog_scale=vlog_max;
	} else if(vlog_scale<vlog_min) {
		vlog_scale=vlog_min;
	}
	virtualLogScale=vlog_scale;
}

double ImageViewer::actualScale(void)const
{
	return baseScale*virtualScale();
}

//viewport -> item
QPointF ImageViewer::mapToItem(int x, int y)const
{
	return mapToItem(QPoint(x,y));
}

QPointF ImageViewer::mapToItem(const QPoint& point)const
{
	const auto diff=QPoint(point.x()-pixmapRect.x(), point.y()-pixmapRect.y());
	return diff/actualScale();
}

//item -> viewport
QPointF ImageViewer::mapFromItem(qreal x,qreal y)const
{
	return mapFromItem(QPointF(x, y));
}

QPointF ImageViewer::mapFromItem(const QPointF& point)const
{
	return QPointF(pixmapRect.x()+point.x()*actualScale(), pixmapRect.y()+point.y()*actualScale());
}


void ImageViewer::updatePixmapRect(void)
{
	pixmapRect.setWidth(int(rawPixmap.width()*actualScale()));
	pixmapRect.setHeight(int(rawPixmap.height()*actualScale()));
}

void ImageViewer::updatePixmapRect(const QPointF& pos)
{
	updatePixmapRect(pos.x(), pos.y());
}

void ImageViewer::updatePixmapRect(double x,double y)
{
	pixmapRect.setX(x);
	pixmapRect.setY(y);

	updatePixmapRect();
}


void ImageViewer::paintEvent(QPaintEvent *event)
{
	QPainter painter(viewport());
	painter.setRenderHint(QPainter::SmoothPixmapTransform,true);

	painter.drawPixmap(pixmapRect, rawPixmap);
}


void ImageViewer::resizeEvent(QResizeEvent *event)
{
	//const auto& available=QGuiApplication::primaryScreen()->availableSize();
	const double xscale=(double)event->size().width()/rawPixmap.width();
	const double yscale=(double)event->size().height()/rawPixmap.height();
	baseScale=1.0;
	if(baseScale>xscale) {
		baseScale=xscale;
	}
	if(baseScale>yscale) {
		baseScale=yscale;
	}
	virtualLogScale=0.0;

	positionMapping(QPointF(rawPixmap.width(), rawPixmap.height())/2, QPointF(event->size().width(), event->size().height())/2);
}


void	ImageViewer::mousePressEvent(QMouseEvent *event)
{
	if(event->button()==Qt::LeftButton) {
		leftClick=true;
	}
}

void	ImageViewer::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button()==Qt::LeftButton) {
		leftClick=false;
	}
}

void	ImageViewer::mouseMoveEvent(QMouseEvent *event)
{
	if(leftClick) {
		const QPointF delta=(event->pos()-mousePos);
		updatePixmapRect(pixmapRect.x()+delta.x(), pixmapRect.y()+delta.y());
		adjustPosition();
		viewport()->repaint();
	}
	mousePos=event->pos();
}


void	ImageViewer::wheelEvent(QWheelEvent *event)
{
	/*
	log_a(virtualScale) =∫steps dt となるようにしたい。
	d(log_a(virtualScale))/dt = steps
	*/
	const double prevLogScale=virtualLogScale;
	const double steps=event->angleDelta().y()/120.0;

	auto onPixmap=mapToItem(mousePos);
	auto onScreen=mousePos;
	if(onPixmap.x()<0||rawPixmap.width()<onPixmap.x()) {
		onScreen.setX(width()/2);
		onPixmap.setX(rawPixmap.width()/2);
	}
	if(onPixmap.y()<0||rawPixmap.height()<onPixmap.y()) {
		onScreen.setY(height()/2);
		onPixmap.setY(rawPixmap.height()/2);
	}

	setVirtualLogScale(virtualLogScale+steps*0.1);

	//scale changed
	if(prevLogScale!=virtualLogScale) {

		updatePixmapRect();

		positionMapping(onPixmap, onScreen);
		adjustPosition();

		viewport()->repaint();
	}
}


void ImageViewer::adjustPosition(void)
{
	//if the image width is smaller than that of screen, move it to the center
	if(pixmapRect.width()<width()) {
		pixmapRect.setX((width()-pixmapRect.width())/2);
	} else {	//if the image corner is in screen, match image' edge to that of screen
		if(0<pixmapRect.left()) {
			pixmapRect.setX(0);
		} else if(pixmapRect.right()<width()) {
			pixmapRect.setX(width()-pixmapRect.width());
		}
	}

	//if the image height is smaller than that of screen, move it to the center
	if(pixmapRect.height()<height()) {
		pixmapRect.setY((height()-pixmapRect.height())/2);
	} else {	//if the image corner is in screen, match image' edge to that of screen
		if(0<pixmapRect.top()) {
			pixmapRect.setY(0);
		} else if(pixmapRect.bottom()<height()) {
			pixmapRect.setY(height()-pixmapRect.height());
		}
	}

	updatePixmapRect();
}
