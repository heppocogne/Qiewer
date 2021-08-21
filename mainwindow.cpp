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
	 timer(new QTimer(this))
{
	viewertabs->setTabsClosable(true);
	viewertabs->setMovable(true);
	setCentralWidget(viewertabs);
	
	sharedMemory->create(sharedMemorySize);
	sharedMemory->lock();
	strcpy(reinterpret_cast<char*>(sharedMemory->data()), "");
	sharedMemory->unlock();
	
	connect(viewertabs, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
	connect(timer, &QTimer::timeout, this, &MainWindow::checkSharedMemory);
	
	//check shared memory every 0.25s
	timer->start(250);

	logger.write("read config file:	"+configFile, LOG_FROM);

	std::fstream configStream(configFile, std::ios_base::binary|std::ios_base::in);
	if(configStream) {
		configStream.read(reinterpret_cast<char*>(&config), sizeof(WindowConfigure));
	} else {
		const auto& available=QGuiApplication::primaryScreen()->availableSize();
		config.width=available.width()*0.9;
		config.height=available.height()*0.9;
	}
}

void MainWindow::outputConfig(const std::string& configFile)
{
	std::fstream configStream(configFile, std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
	if(configStream) {
		logger.write("write config file:	"+configFile, LOG_FROM);
		
		if(windowState()&Qt::WindowMaximized) {
			config.width=-1;
			config.height=-1;
		} else {
			config.width=geometry().width();
			config.height=geometry().height();
		}
		configStream.write(reinterpret_cast<const char*>(&config), sizeof(WindowConfigure));
		configStream.close();
	}
}

MainWindow::~MainWindow()
{
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
	if(config.width<0||config.height<0) {
		logger.write("show maximized window", LOG_FROM);
		
		showMaximized();
	} else {
		//resize(config.width, config.height);
		logger.write("show normal window:	"+std::to_string(config.width)+"x"+std::to_string(config.height), LOG_FROM);
		
		const auto& available=QGuiApplication::primaryScreen()->availableSize();
		setGeometry((available.width()-config.width)/2, (available.height()-config.height)/2, config.width, config.height);
		show();
	}
}


void MainWindow::closeTab(int idx)
{
	logger.write("remove tab["+std::to_string(idx)+"]:	"+viewertabs->tabText(idx).toStdString(), LOG_FROM);

	viewertabs->removeTab(idx);
	//idx--;
	if(viewertabs->count()<=0) {
		//close window
		close();
	} else {
		//viewertabs->setCurrentIndex(idx);

		logger.write("current tab["+std::to_string(idx)+"]:	"+viewertabs->tabText(idx).toStdString(), LOG_FROM);
	}
}

void MainWindow::checkSharedMemory(void)
{
	//logger.write("checkSharedMemory()", LOG_FROM);
	
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
	
	//QMainWindow::resizeEvent(event);
	viewertabs->resize(event->size());
}
