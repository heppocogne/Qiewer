#include "svgviewer.h"

#include <QPainter>


SvgViewer::SvgViewer(QWidget* parent)
	:ViewerInterface(parent),
	renderer(new QSvgRenderer(this))
{
	
}


bool SvgViewer::load(const QString& srcImageFile)
{
	if(renderer->load(srcImageFile))
	{
		rawSize=renderer->defaultSize();
		
		return true;
	}else{
		return false;
	}
}

void SvgViewer::paintEvent(QPaintEvent *event)
{
	if(event==nullptr) {}	//warning avoidance
	
	QPainter painter(viewport());
	
	renderer->render(&painter, pixmapRect);
}
