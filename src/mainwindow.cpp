#include "mainwindow.h"
#include <QPixmap>
#include <QString>
#include "imageviewer.h"
#include "logger.h"
#include <QGuiApplication>
#include <QPoint>
#include <QScreen>
#include <cstring>
#include <QMimeData>
#include <QMessageBox>
#include <QCursor>
#include <QIcon>


MainWindow::MainWindow()
	:QMainWindow(nullptr, Qt::Window),
	 imageReader(new QImageReader()),
	 viewertabs(new QTabWidget(this)),
	 toolbar(new QToolBar(this)),
	 fileSelector(new FileSelector(this)),
	 sharedMemory(new QSharedMemory(sharedMemoryKey, this)),
	 sharedMemoryTick(new QTimer(this)),
	 cursorTick(new QTimer(this))
{
	//setup window
	setAcceptDrops(true);


	//setup viewertabs
	viewertabs->setTabsClosable(true);
	viewertabs->setMovable(true);
	viewertabs->setTabPosition(QTabWidget::South);
	setCentralWidget(viewertabs);


	//setup toolbar
	toolbar->setFloatable(false);
	//toolbar->setVisible(true);
	toolbar->setVisible(false);
	toolbar->setStyleSheet("background-color: rgb(240, 240, 240)");
	
	//setup fileselector
	toolbar->addAction(QIcon(":/rc/fopen24.png"), "Open File", fileSelector, &FileSelector::open);
	connect(fileSelector, &FileSelector::fileSelected, this, &MainWindow::addImage);
	

	//setup shared memory
	sharedMemory->create(sharedMemorySize);
	sharedMemory->lock();
	strcpy(reinterpret_cast<char*>(sharedMemory->data()), "");
	sharedMemory->unlock();


	//connect signals
	connect(viewertabs, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
	connect(sharedMemoryTick, &QTimer::timeout, this, &MainWindow::checkSharedMemory);
	connect(cursorTick, &QTimer::timeout, this, &MainWindow::checkMousePosition);

	//check shared memory every 0.25s
	sharedMemoryTick->start(250);

	//check mouse position every 0.1s
	cursorTick->start(0.1);
}


MainWindow::~MainWindow()
{
	sharedMemoryTick->stop();
	cursorTick->stop();

	delete imageReader;
}


bool MainWindow::addImage(const QString& imageFileName)
{
	logger.write("add image:	"+imageFileName, LOG_FROM);

	imageReader->setFileName(imageFileName);

	if(imageReader->canRead()) {
		//the given file is readable
		ImageViewer* const viewer=new ImageViewer(viewertabs);

		const auto& image=imageReader->read();
		viewer->setImage(image);
		const int idx=viewertabs->addTab(viewer, extractFileName(imageFileName));
		viewertabs->setCurrentIndex(idx);
		
		return true;
	} else {
		const QString msg="Qiewer does not support this file format";
		logger.write(msg, LOG_FROM);
		QMessageBox::warning(this, "Warning: Unknown file format", msg);

		return false;
	}
}


void MainWindow::showProperly(void)
{
	if(configureIO.config.maximized) {
		logger.write("show maximized window", LOG_FROM);

		showMaximized();
	} else {
		logger.write("show normal window:	"+QString::number(configureIO.config.width)+"x"+QString::number(configureIO.config.height), LOG_FROM);

		const auto& available=QGuiApplication::primaryScreen()->availableSize();
		setGeometry((available.width()-configureIO.config.width)/2, (available.height()-configureIO.config.height)/2,
		            configureIO.config.width, configureIO.config.height);
		show();
	}
}


void MainWindow::closeTab(int idx)
{
	logger.write("remove tab["+QString::number(idx)+"]:	"+viewertabs->tabText(idx), LOG_FROM);

	viewertabs->removeTab(idx);
	if(viewertabs->count()<=0) {
		//close window
		close();
	} else {
		logger.write("current tab["+QString::number(idx)+"]:	"+viewertabs->tabText(idx), LOG_FROM);
	}
}

void MainWindow::checkSharedMemory(void)
{
	sharedMemory->lock();
	char* input=reinterpret_cast<char*>(sharedMemory->data());
	if(input!=nullptr&&0<strlen(input)) {
		addImage(input);
		strcpy(input, "");
	}
	sharedMemory->unlock();
}


void MainWindow::checkMousePosition(void)
{
	const auto pos_global=QCursor::pos();
	/*
	const auto pos_window=this->mapFromGlobal(pos_global);
	const auto pos_toolbar=toolbar->mapFromGlobal(pos_global);
	*/
	//this->geometry() is on the global coordinates
	//toolbar->geometry() is on the window coordinates
	
	const auto pos_window=this->mapFromGlobal(pos_global);
	if(geometry().contains(pos_global) && pos_window.y()<toolbar->geometry().bottom()*2) {
		//logger.write("pos_global=["+QString::number(pos_global.x())+", "+QString::number(pos_global.y())+"]", LOG_FROM);
		toolbar->setVisible(true);
	} else {
		toolbar->setVisible(false);
	}
}


void MainWindow::resizeEvent(QResizeEvent *event)
{
	logger.write("window resized:	"+QString::number(event->size().width())+"x"+QString::number(event->size().height()), LOG_FROM);
	
	//toolbar->geometry().moveRight(event->size().width());
	auto toolbarGeometry=toolbar->geometry();
	toolbarGeometry.setLeft(0);
	toolbarGeometry.setRight(event->size().width());
	toolbar->setGeometry(toolbarGeometry);

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


void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	if(event->mimeData()->hasUrls()) {
		event->acceptProposedAction();
	}
}


void MainWindow::dropEvent(QDropEvent *event)
{
	if(event->mimeData()->urls().size()<configureIO.config.dropFilesLimit) {
		for(const auto& url: event->mimeData()->urls()) {
			addImage(url.toLocalFile());
		}
	} else {
		const QString msg="Qiewer cannot accept more than "+QString::number(configureIO.config.dropFilesLimit)+" files!";
		logger.write(msg, LOG_FROM);
		QMessageBox::warning(this, "Warning: Too much files", msg);
	}
}
