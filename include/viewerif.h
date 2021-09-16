#ifndef VIEWERIF_H_INCLUDED
#define VIEWERIF_H_INCLUDED

#include <QWidget>
#include <QSize>
#include <QRect>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QGraphicsView>
#include <QPoint>
#include <QString>


class ViewerInterface: public QGraphicsView
{
		Q_OBJECT
		
	protected:
		QRect pixmapRect;	//used to draw pixmap
		QSize rawSize;
		QString filename;
		
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

		bool viewActualSize;

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
		
		void unknownFormatErrorDialog(void);
		
		virtual bool load(const QString&)=0;
		virtual void	mouseMoveEvent(QMouseEvent *event);
		virtual void	mousePressEvent(QMouseEvent *event);
		virtual void	mouseReleaseEvent(QMouseEvent *event);
		virtual void	resizeEvent(QResizeEvent *event);
		virtual void	wheelEvent(QWheelEvent *event);
		virtual void	paintEvent(QPaintEvent *event)=0;

	public:
		ViewerInterface(QWidget* parent=nullptr);
		virtual ~ViewerInterface() {}
		virtual QSize sizeHint(void)const;
		const QString& getFileName(void)const;

	//public slots:	//public functions
		bool setImageFile(const QString& srcImageFile);
		void fitSize(void);	//fit to screen; virtualScale=1
		void actualSize(void);	//view original size; actualScale=1
		void zoom(int value);
		bool reload(void);
};


#endif
