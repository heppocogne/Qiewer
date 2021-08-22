#include "mainwindow.h"
#include <QPixmap>
#include <QString>
#include "imageviewer.h"
#include "logger.h"
#include <QGuiApplication>
#include <QPoint>
#include <QScreen>
#include <cstring>


const char* MainWindow::sharedMemoryKey="Qiewer/image/input";
const size_t MainWindow::sharedMemorySize=1024;


MainWindow::MainWindow(const std::string& configFile)
	:QMainWindow(nullptr, Qt::Window),
	 imageReader(new QImageReader()),
	 viewertabs(new QTabWidget(this)),
	 sharedMemory(new QSharedMemory(sharedMemoryKey, this)),
	 timer(new QTimer(this)),
	 configureIO(configFile),
	 firstImage(true)
{
	viewertabs->setTabsClosable(true);
	viewertabs->setMovable(true);
	viewertabs->setTabPosition(QTabWidget::South);
	setCentralWidget(viewertabs);

	sharedMemory->create(sharedMemorySize);
	sharedMemory->lock();
	strcpy(reinterpret_cast<char*>(sharedMemory->data()), "");
	sharedMemory->unlock();

	connect(viewertabs, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
	connect(timer, &QTimer::timeout, this, &MainWindow::checkSharedMemory);

	//check shared memory every 0.25s
	timer->start(250);

	configureIO.load();
}


MainWindow::~MainWindow()
{
	//saveConfig();
	//configureIO.save();
	delete imageReader;
}

std::string MainWindow::extractFileName(const std::string& fileName)
{
	int idx=fileName.length()-1;
	for(; 0<=idx; idx--) {
		if(fileName[idx]=='\\'||fileName[idx]=='/') {
			break;
		}
	}
	//idx=-1 or fileName[idx]=\ or /
	return fileName.substr(idx+1);
}

void MainWindow::addImage(const std::string& imageFileName)
{
	logger.write("add image:	"+imageFileName, LOG_FROM);

	imageReader->setFileName(imageFileName.c_str());

	if(imageReader->canRead()) {
		//the given file is readable
		ImageViewer* const viewer=new ImageViewer(viewertabs);

		const auto& image=imageReader->read();
		viewer->setImage(image);
		viewertabs->addTab(viewer, extractFileName(imageFileName).c_str());
	}
}


void MainWindow::showProperly(void)
{
	if(configureIO.config.maximized) {
		logger.write("show maximized window", LOG_FROM);

		showMaximized();
	} else {
		logger.write("show normal window:	"+std::to_string(configureIO.config.width)+"x"+std::to_string(configureIO.config.height), LOG_FROM);

		const auto& available=QGuiApplication::primaryScreen()->availableSize();
		setGeometry((available.width()-configureIO.config.width)/2, (available.height()-configureIO.config.height)/2,
		            configureIO.config.width, configureIO.config.height);
		show();
	}
}


void MainWindow::closeTab(int idx)
{
	logger.write("remove tab["+std::to_string(idx)+"]:	"+viewertabs->tabText(idx).toStdString(), LOG_FROM);

	viewertabs->removeTab(idx);
	if(viewertabs->count()<=0) {
		//close window
		close();
	} else {
		logger.write("current tab["+std::to_string(idx)+"]:	"+viewertabs->tabText(idx).toStdString(), LOG_FROM);
	}
}

void MainWindow::checkSharedMemory(void)
{
	sharedMemory->lock();
	char* input=reinterpret_cast<char*>(sharedMemory->data());
	if(input!=nullptr&&0<strlen(input)) {
		addImage(std::string(input));
		strcpy(input, "");
	}
	sharedMemory->unlock();
}


void MainWindow::resizeEvent(QResizeEvent *event)
{
	logger.write("window resized:	"+std::to_string(event->size().width())+"x"+std::to_string(event->size().height()), LOG_FROM);

	configureIO.config.width=event->size().width();
	configureIO.config.height=event->size().height();
}


void MainWindow::changeEvent(QEvent* event)
{
	switch(event->type()) {
		case  QEvent::WindowStateChange:
			if(windowState()==Qt::WindowMaximized) {
				configureIO.config.maximized=true;
			} else if(windowState()==Qt::WindowNoState) {
				configureIO.config.maximized=false;
			}
		default:
			//ignore
			do {} while(false);
	}
}
