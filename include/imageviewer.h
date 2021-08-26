#ifndef IMAGEVIEWER_H_INCLUDED
#define IMAGEVIEWER_H_INCLUDED

#include <QWidget>
#include <QSize>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QPoint>


class ImageViewer: public QGraphicsView
{
		Q_OBJECT
		QPixmap rawPixmap;
		QRect pixmapRect;	//used to draw pixmap
		void updatePixmapRect(void);	//update pixmapRect according to scale
		void updatePixmapRect(const QPointF& pos);	//set position and update
		void updatePixmapRect(double x,double y);
		
		//static Qt::TransformationMode transformationMode;
		bool leftClick;
		QPoint mousePos;
		static const double virtualScaleMax;
		static const double virtualScaleMin;
		double baseScale;	//image scale to fit to draw area (not affect coordinate system)
		double virtualLogScale;	//relative scale to draw area (〃)

		void updateBaseScale(void);
		double virtualScale(void)const;
		void setVirtualScale(double v_scale);
		void setVirtualLogScale(double v_scale);
		double actualScale(void)const;
		
		//map to pixmap (pixmap coordinate; left-top origin, same scale as raw pixmap)
		QPointF mapToItem(int x, int y)const;
		QPointF mapToItem(const QPoint& point)const;

		//map from pixmap
		QPointF mapFromItem(qreal x,qreal y)const;
		QPointF mapFromItem(const QPointF& point)const;
		
		//move the pixmap so that match 'pixmapPos' to 'screenPos'
		void positionMapping(const QPointF& pixmapPos, const QPointF& screenPos);
		void adjustPosition(void);
		void zoomMain(int steps, const QPoint& onScreen);

	public:
		ImageViewer(QWidget* parent);
		//~ImageViewer();
		virtual QSize sizeHint(void)const;
		//QString filename;
	
	public slots:
		void setImage(const QImage& image);
		void fitToWindow(void);	//fit to screen; virtualScale=1
		void displayOriginal(void);	//view original size; actualScale=1
		/*
		void zoomin(int value);
		void zoomout(int value);
		*/
		void zoom(int value);

	protected:
		virtual void	mouseMoveEvent(QMouseEvent *event);
		virtual void	mousePressEvent(QMouseEvent *event);
		virtual void	mouseReleaseEvent(QMouseEvent *event);
		virtual void	resizeEvent(QResizeEvent *event);
		virtual void	wheelEvent(QWheelEvent *event);
		virtual void	paintEvent(QPaintEvent *event);
};

#endif
