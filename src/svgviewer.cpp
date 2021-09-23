#include "svgviewer.h"

#include <QPainter>

#include "configure.h"
#include <cmath>


SvgViewer::SvgViewer(QWidget* parent)
	:ViewerInterface(parent),
	 renderer(new QSvgRenderer(this))
{

}


void SvgViewer::setVirtualScale(double v_scale)
{
	if(configure.svg_scalingUnlimited)
		virtualLogScale=std::log10(v_scale);
	else
		ViewerInterface::setVirtualScale(v_scale);
}

void SvgViewer::setVirtualLogScale(double v_scale)
{
	if(configure.svg_scalingUnlimited)
		virtualLogScale=v_scale;
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

	//pixmapRect size must be larger than 1x1
	if(pixmapRect.width()<=0)
		pixmapRect.setWidth(1);
	if(pixmapRect.height()<=0)
		pixmapRect.setHeight(1);

	renderer->render(&painter, pixmapRect);
}
