#include "viewerif.h"

#include <Qt>
#include <QMessageBox>


#include "logger.h"
#include <cmath>


double ViewerInterface::virtualScaleMax=10.0;
double ViewerInterface::virtualScaleMin=0.1;

ViewerInterface::ViewerInterface(QWidget *parent)
	:QGraphicsView(parent),
	 rightClickMenu(new QMenu(this)),
	 zoominAction(new QAction("Zoom In", rightClickMenu)),
	 zoomoutAction(new QAction("Zoom Out", rightClickMenu)),
	 fitToWindowAction(new QAction("Fit to Window", rightClickMenu)),
	 actualSizeAction(new QAction("Actual Size", rightClickMenu)),
	 closeAction(new QAction("Close", rightClickMenu)),
	 leftClick(false),
	 baseScale(1.0),
	 virtualLogScale(0.0),
	 viewActualSize(false)
{
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setCacheMode(QGraphicsView::CacheBackground);
	setMouseTracking(true);

	//setup context menu
	setContextMenuPolicy(Qt::CustomContextMenu);
	/*
	Zoom In
	Zoom Out
	Fit to Window
	Actual Size
	----------------
	Close
	*/
	rightClickMenu->addAction(zoominAction);
	rightClickMenu->addAction(zoomoutAction);
	rightClickMenu->addAction(fitToWindowAction);
	rightClickMenu->addAction(actualSizeAction);
	rightClickMenu->addSeparator();
	rightClickMenu->addAction(closeAction);

	connect(this, &ViewerInterface::customContextMenuRequested, [&] {rightClickMenu->exec(mousePos);});
	connect(zoominAction, &QAction::triggered, [&] {zoom(1);});
	connect(zoomoutAction, &QAction::triggered, [&] {zoom(-1);});
	connect(fitToWindowAction, &QAction::triggered, this, fitSize);
	connect(actualSizeAction, &QAction::triggered, this, actualSize);
	connect(closeAction, &QAction::triggered, [&]{emit closeMe(this);});
}


bool ViewerInterface::setImageFile(const QString& srcImageFile)
{
	filename=srcImageFile;

	if(load(srcImageFile)) {
		baseScale=1.0;
		virtualLogScale=0.0;

		updatePixmapRect();
		positionMapping(QPointF(rawSize.width(), rawSize.height())/2, QPointF(width(), height())/2);

		return true;
	} else {
		return false;
	}
}

bool ViewerInterface::reload(void)
{
	return setImageFile(filename);
}

const QString& ViewerInterface::getFileName(void)const
{
	return filename;
}


void ViewerInterface::positionMapping(const QPointF& pixmapPos, const QPointF& screenPos)
{
	const auto delta=screenPos-mapFromItem(pixmapPos);
	updatePixmapRect(pixmapRect.x()+delta.x(), pixmapRect.y()+delta.y());
}


QSize ViewerInterface::sizeHint(void)const
{
	return QSize(pixmapRect.width(), pixmapRect.height());
}


double ViewerInterface::getVirtualScale(void)const
{
	return std::pow(10, virtualLogScale);
}

void ViewerInterface::setVirtualScale(double v_scale)
{
	if(1.0<baseScale*v_scale && virtualScaleMax<v_scale) {
		v_scale=virtualScaleMax;
	} else if(v_scale<virtualScaleMin) {
		v_scale=virtualScaleMin;
	}
	virtualLogScale=std::log10(v_scale);
}

void ViewerInterface::setVirtualLogScale(double vlog_scale)
{
	const double vlog_max=std::log10(virtualScaleMax);
	const double vlog_min=std::log10(virtualScaleMin);
	if(1.0<baseScale*std::pow(10, vlog_scale) && vlog_max<vlog_scale) {
		vlog_scale=vlog_max;
	} else if(vlog_scale<vlog_min) {
		vlog_scale=vlog_min;
	}
	virtualLogScale=vlog_scale;
}

double ViewerInterface::getActualScale(void)const
{
	return baseScale*getVirtualScale();
}

//viewport -> item
QPointF ViewerInterface::mapToItem(int x, int y)const
{
	return mapToItem(QPoint(x,y));
}

QPointF ViewerInterface::mapToItem(const QPoint& point)const
{
	const auto diff=QPoint(point.x()-pixmapRect.x(), point.y()-pixmapRect.y());
	return diff/getActualScale();
}
//item -> viewport
QPointF ViewerInterface::mapFromItem(qreal x,qreal y)const
{
	return mapFromItem(QPointF(x, y));
}

QPointF ViewerInterface::mapFromItem(const QPointF& point)const
{
	return QPointF(pixmapRect.x()+point.x()*getActualScale(), pixmapRect.y()+point.y()*getActualScale());
}


void ViewerInterface::updatePixmapRect(void)
{
	pixmapRect.setWidth(int(rawSize.width()*getActualScale()));
	pixmapRect.setHeight(int(rawSize.height()*getActualScale()));
}

void ViewerInterface::updatePixmapRect(const QPointF& pos)
{
	updatePixmapRect(pos.x(), pos.y());
}

void ViewerInterface::updatePixmapRect(double x,double y)
{
	pixmapRect.setX(x);
	pixmapRect.setY(y);

	updatePixmapRect();
}

void ViewerInterface::updateBaseScale(void)
{
	const double xscale=(double)width()/rawSize.width();
	const double yscale=(double)height()/rawSize.height();

	baseScale=1.0;
	if(baseScale>xscale) {
		baseScale=xscale;
	}
	if(baseScale>yscale) {
		baseScale=yscale;
	}
}


void ViewerInterface::resizeEvent(QResizeEvent *event)
{
	const auto lookat=mapToItem(event->oldSize().width()/2, event->oldSize().height()/2);
	updateBaseScale();
	if(viewActualSize) {
		setVirtualScale(1.0/baseScale);
	}
	positionMapping(lookat, QPointF(event->size().width(), event->size().height())/2);
	adjustPosition();
}


void	ViewerInterface::mousePressEvent(QMouseEvent *event)
{
	if(event->button()==Qt::LeftButton) {
		leftClick=true;
	}
}

void	ViewerInterface::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button()==Qt::LeftButton) {
		leftClick=false;
	}
}

void	ViewerInterface::mouseMoveEvent(QMouseEvent *event)
{
	if(leftClick) {
		const QRect prevPixmapRect=pixmapRect;
		const QPointF delta=(event->pos()-mousePos);
		updatePixmapRect(pixmapRect.x()+delta.x(), pixmapRect.y()+delta.y());
		adjustPosition();
		if(prevPixmapRect!=pixmapRect) {
			viewport()->repaint();
		}
	}
	mousePos=event->pos();
}


void	ViewerInterface::wheelEvent(QWheelEvent *event)
{
	/*
	log_a(virtualScale) =∫steps dt となるようにしたい。
	d(log_a(virtualScale))/dt = steps
	*/
	const double steps=event->angleDelta().y()/120.0;

	auto onScreen=mousePos;
	const auto onPixmap=mapToItem(mousePos);
	if(onPixmap.x()<0||rawSize.width()<onPixmap.x()) {
		onScreen.setX(width()/2);
	}
	if(onPixmap.y()<0||rawSize.height()<onPixmap.y()) {
		onScreen.setY(height()/2);
	}
	zoomMain(steps, onScreen);
}


void ViewerInterface::adjustPosition(void)
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


void ViewerInterface::fitSize(void)
{
	viewActualSize=false;

	if(virtualLogScale!=0.0) {
		const auto onPixmap=QPoint(rawSize.width()/2, rawSize.height()/2);
		const auto onScreen=QPoint(width()/2, height()/2);
		setVirtualLogScale(0.0);
		updatePixmapRect();

		positionMapping(onPixmap, onScreen);
		adjustPosition();

		viewport()->repaint();
	}
}

void ViewerInterface::actualSize(void)
{
	viewActualSize=true;

	if(getActualScale()!=1.0) {
		const auto onScreen=QPoint(width()/2, height()/2);
		const auto onPixmap=mapToItem(onScreen);
		setVirtualScale(1.0/baseScale);
		updatePixmapRect();

		positionMapping(onPixmap, onScreen);
		adjustPosition();

		viewport()->repaint();
	}
}

void ViewerInterface::zoom(int value)
{
	zoomMain(value, QPoint(width()/2, height()/2));
}


void ViewerInterface::zoomMain(int steps, const QPoint& onScreen)
{
	viewActualSize=false;

	const double prevLogScale=virtualLogScale;
	const auto onPixmap=mapToItem(onScreen);

	setVirtualLogScale(virtualLogScale+steps*0.1);

	//scale changed
	if(prevLogScale!=virtualLogScale) {

		updatePixmapRect();

		positionMapping(onPixmap, onScreen);
		adjustPosition();

		viewport()->repaint();
	}
}


void ViewerInterface::unknownFormatErrorDialog(void)
{
	const QString msg="Qiewer does not support this file format";
	logger.write(msg, LOG_FROM);
	QMessageBox::warning(this, "Warning: Unknown file format", msg);
}
