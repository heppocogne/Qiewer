#ifndef SVGVIEWER_H_INCLUDED
#define SVGVIEWER_H_INCLUDED

#include <QWidget>
#include <QSvgRenderer>
#include <QString>
#include <QPaintEvent>

#include "viewerif.h"


class SvgViewer: public ViewerInterface
{
		Q_OBJECT
		
		QSvgRenderer* const renderer;
		
	public:
		SvgViewer(QWidget* parent=nullptr);
		virtual ~SvgViewer() {}

	protected:
		virtual bool load(const QString& srcImageFile);
		virtual void paintEvent(QPaintEvent *event);
};


#endif
