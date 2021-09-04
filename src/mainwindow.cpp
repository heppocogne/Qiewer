#include "mainwindow.h"

#include <Qt>
#include <QPixmap>
#include <QString>
#include <QGuiApplication>
#include <QPoint>
#include <QScreen>
#include <QMimeData>
#include <QMessageBox>
#include <QCursor>
#include <QIcon>
#include <QWindowStateChangeEvent>

#include "imageviewer.h"
#include "nameutil.h"
#include "logger.h"
#include <chrono>
#include <cstring>
#if defined(_WIN32) || defined(_WIN64)
	#include <windows.h>
#endif


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
	toolbar->setVisible(false);
	toolbar->setStyleSheet("background-color: rgb(240, 240, 240)");

	//setup fileselector icon
	toolbar->addAction(QIcon(":/rc/fopen24.png"), "Open File", fileSelector, &FileSelector::open);
	connect(fileSelector, &FileSelector::fileSelected, this, &MainWindow::addImage);

	//setup reload icon

	//setup zoomin icon
	toolbar->addSeparator();
	toolbar->addAction(QIcon(":/rc/zoomin24.png"), "Zoom In", this, &MainWindow::zoomin);

	//setup zooout icon
	toolbar->addAction(QIcon(":/rc/zoomout24.png"), "Zoom Out", this, &MainWindow::zoomout);

	//setup view actual size icon
	toolbar->addAction(QIcon(":/rc/100%24.png"), "Actual Size", this, &MainWindow::actualSize);

	//setup fit size icon
	toolbar->addAction(QIcon(":/rc/fit24.png"), "Fit to Window", this, &MainWindow::fitSize);

	//setup setting icon


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
	sharedMemoryTick->start(std::chrono::milliseconds(250));

	//check mouse position every 0.025s
	cursorTick->start(std::chrono::milliseconds(25));
}


MainWindow::~MainWindow()
{
	sharedMemoryTick->stop();
	cursorTick->stop();

	delete imageReader;
}


ImageViewer* MainWindow::currentView(void)const
{
	if(viewertabs->count()>0) {
		return static_cast<ImageViewer*>(viewertabs->currentWidget());
	} else {
		return nullptr;
	}
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
		viewer->filename=imageFileName;
		const int idx=viewertabs->addTab(viewer, extractFileName(imageFileName));
		viewertabs->setCurrentIndex(idx);
		
		setWindowState(windowState()&~Qt::WindowMinimized);
		#if defined(_WIN32) || defined(_WIN64)
			//bring the window to the top (Windows depenednt code)
			const HWND hWnd=reinterpret_cast<HWND>(winId());
			SetWindowPos(hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
			SetWindowPos(hWnd,HWND_NOTOPMOST,0,0,0,0,SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
		#else
			//I don't know whether this actually works
			setWindowState(windowState()|Qt::WindowActive);
		#endif
		
		return true;
	} else {
		const QString msg="Qiewer does not support this file format";
		logger.write(msg, LOG_FROM);
		QMessageBox::warning(this, "Warning: Unknown file format", msg);

		return false;
	}
}

void MainWindow::reload(void)
{
	//ImageViewer* const viewer=static_cast<ImageViewer*>(viewertabs->currentWidget());
	ImageViewer* const viewer=currentView();
	if(viewer) {
		imageReader->setFileName(viewer->filename);
		if(imageReader->canRead()) {
			viewer->setImage(imageReader->read());
		}
	}
}

void MainWindow::fitSize(void)
{
	ImageViewer* const viewer=currentView();
	if(viewer) {
		viewer->fitSize();
	}
}

void MainWindow::actualSize(void)
{
	ImageViewer* const viewer=currentView();
	if(viewer) {
		viewer->actualSize();
	}
}

void  MainWindow::zoomin(void)
{
	zoom(1);
}

void MainWindow::zoomout(void)
{
	zoom(-1);
}

void MainWindow::zoom(int value)
{
	ImageViewer* const viewer=currentView();
	if(viewer) {
		viewer->zoom(value);
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
		const QString imageFileName=QString::fromLocal8Bit(input);
		addImage(imageFileName);
		strcpy(input, "");
	}
	sharedMemory->unlock();
}


void MainWindow::checkMousePosition(void)
{
	const auto pos_global=QCursor::pos();
	const auto pos_window=this->mapFromGlobal(pos_global);
	if(geometry().contains(pos_global) && pos_window.y()<toolbar->geometry().bottom()*2) {
		if(!toolbar->isVisible()) {
			auto toolbarGeometry=toolbar->geometry();
			toolbarGeometry.setLeft(0);
			toolbarGeometry.setRight(width());
			toolbar->setGeometry(toolbarGeometry);
		}

		toolbar->setVisible(true);
	} else {
		toolbar->setVisible(false);
	}
}


void MainWindow::resizeEvent(QResizeEvent *event)
{
	logger.write("window resized:	"+QString::number(event->size().width())+"x"+QString::number(event->size().height()), LOG_FROM);

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
			if(windowState()&Qt::WindowMaximized) {
				configureIO.config.maximized=true;
			} else {
				configureIO.config.maximized=false;
			}
			break;
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
