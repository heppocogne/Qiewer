#include "svgviewer.h"

#include <QPainter>

#include <cmath>


bool SvgViewer::scalingUnlimited=false;


SvgViewer::SvgViewer(QWidget* parent)
	:ViewerInterface(parent),
	 renderer(new QSvgRenderer(this))
{

}


void SvgViewer::setVirtualScale(double v_scale)
{
	if(scalingUnlimited)
	{
		//pixmapRect size must be larger than 1x1
		virtualLogScale=std::log10(v_scale);
	}
	else
		ViewerInterface::setVirtualScale(v_scale);
}

void SvgViewer::setVirtualLogScale(double v_scale)
{
	if(scalingUnlimited)
	{
		//pixmapRect size must be larger than 1x1
		virtualLogScale=v_scale;
	}
	else
		ViewerInterface::setVirtualLogScale(v_scale);
}


bool SvgViewer::load(const QString& srcImageFile)
{
	if(renderer->load(srcImageFile)) {
		rawSize=renderer->defaultSize();

		return true;
	} else {
		return false;
	}
}

void SvgViewer::paintEvent(QPaintEvent *event)
{
	if(event==nullptr) {}	//warning avoidance

	QPainter painter(viewport());

	renderer->render(&painter, pixmapRect);
}
