#include "mainwindow.h"

#include <Qt>
#include <QString>
#include <QGuiApplication>
#include <QImageReader>
#include <QPoint>
#include <QScreen>
#include <QMimeData>
#include <QMessageBox>
#include <QCursor>
#include <QIcon>
#include <QWindowStateChangeEvent>

#include "imageviewer.h"
#include "svgviewer.h"
#include "nameutil.h"
#include "logger.h"
#include <chrono>
#include <cstring>
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif


MainWindow::MainWindow(QWidget* parent)
	:QMainWindow(parent, Qt::Window),
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
	//setup setting icon

	//setup zoomin icon
	toolbar->addSeparator();
	toolbar->addAction(QIcon(":/rc/zoomin24.png"), "Zoom In", [&] {zoom(1);});

	//setup zooout icon
	toolbar->addAction(QIcon(":/rc/zoomout24.png"), "Zoom Out",  [&] {zoom(-1);});

	//setup view actual size icon
	toolbar->addAction(QIcon(":/rc/100%24.png"), "Actual Size", this, &MainWindow::actualSize);

	//setup fit size icon
	toolbar->addAction(QIcon(":/rc/fit24.png"), "Fit to Window", this, &MainWindow::fitSize);


	//setup shared memory
	sharedMemory->create(sharedMemorySize);
	sharedMemory->lock();
	strcpy(reinterpret_cast<char*>(sharedMemory->data()), "");
	sharedMemory->unlock();


	//connect signals
	connect(viewertabs, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
	connect(sharedMemoryTick, &QTimer::timeout, this, &MainWindow::checkSharedMemory);
	connect(cursorTick, &QTimer::timeout, this, &MainWindow::checkMousePosition);

	//check shared memory every 0.2s
	sharedMemoryTick->start(std::chrono::milliseconds(200));

	//check mouse position every 0.025s
	cursorTick->start(std::chrono::milliseconds(25));
}


MainWindow::~MainWindow()
{
	sharedMemoryTick->stop();
	cursorTick->stop();
}


ViewerInterface* MainWindow::currentView(void)const
{
	if(viewertabs->count()>0) {
		return static_cast<ViewerInterface*>(viewertabs->currentWidget());
	} else {
		return nullptr;
	}
}


bool MainWindow::addImage(const QString& imageFileName)
{
	logger.write("add image:	"+imageFileName, LOG_FROM);

	int idx;
	for(idx=0; idx<viewertabs->count(); idx++) {
		if(static_cast<ViewerInterface*>(viewertabs->widget(idx))->getFileName()==imageFileName) {
			break;
		}
	}

	if(configureIO.config.allowDuplicatedFiles || viewertabs->count()<=idx) {
		const auto imageFormat=QImageReader::imageFormat(imageFileName);

		logger.write("format="+QString(imageFormat), LOG_FROM);

		ViewerInterface* viewer;
		if(imageFormat=="svg" || imageFormat=="svgz") {
			viewer=new SvgViewer(viewertabs);
		} else {
			viewer=new ImageViewer(viewertabs);
		}
		connect(viewer, &ViewerInterface::closeMe, this, &MainWindow::viewerCloseRequested);

		//ViewerInterface* const viewer=new ImageViewer(viewertabs);
		if(viewer->setImageFile(imageFileName)) {
			idx=viewertabs->addTab(viewer, extractFileName(imageFileName));
		} else {
			return false;
		}
	}

	viewertabs->setCurrentIndex(idx);

	setWindowState(windowState()&~Qt::WindowMinimized);
	if(configureIO.config.maximized) {
		setWindowState(windowState()&Qt::WindowMaximized);
	}

#if defined(_WIN32) || defined(_WIN64)
	//bring the window to the top (Windows depenednt code)
	const HWND hWnd=reinterpret_cast<HWND>(winId());
	SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
#else
	//I don't know whether this actually works
	setWindowState(windowState()|Qt::WindowActive);
#endif

	return true;
}

void MainWindow::viewerCloseRequested(ViewerInterface* viewer)
{
	this->closeTab(viewertabs->indexOf(viewer));
}

void MainWindow::reload(void)
{
	ViewerInterface* const viewer=currentView();
	if(viewer) {
		//viewer->setImageFile(viewer->getFileName());
		viewer->reload();
	}
}

void MainWindow::fitSize(void)
{
	ViewerInterface* const viewer=currentView();
	if(viewer) {
		viewer->fitSize();
	}
}

void MainWindow::actualSize(void)
{
	ViewerInterface* const viewer=currentView();
	if(viewer) {
		viewer->actualSize();
	}
}

void MainWindow::zoom(int value)
{
	ViewerInterface* const viewer=currentView();
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
		idx=viewertabs->currentIndex();
		logger.write("current tab["+QString::number(idx)+"]:	"+viewertabs->tabText(idx), LOG_FROM);
	}
}

void MainWindow::checkSharedMemory(void)
{
	sharedMemory->lock();
	char* input=reinterpret_cast<char*>(sharedMemory->data());
	if(input!=nullptr && 0<strlen(input)) {
		addImage(QString::fromLocal8Bit(input));
		strcpy(input, "");
	}
	sharedMemory->unlock();
}


void MainWindow::checkMousePosition(void)
{
	const auto pos_global=QCursor::pos();
	const auto pos_window=this->mapFromGlobal(pos_global);
	if(geometry().left()<=pos_global.x() &&
	   pos_global.x()<=geometry().right() &&
	   -toolbar->geometry().height()<=pos_window.y() &&
	   pos_window.y()<=toolbar->geometry().bottom()*2) {

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
			addImage(url.toLocalFile().replace('/', '\\'));
		}
	} else {
		const QString msg="Qiewer cannot accept more than "+QString::number(configureIO.config.dropFilesLimit)+" files!";
		logger.write(msg, LOG_FROM);
		QMessageBox::warning(this, "Warning: Too much files", msg);
	}
}


void MainWindow::closeEvent(QCloseEvent *event)
{
	if((!configureIO.config.confirmBeforeQuit) || viewertabs->count()<=1 || configureIO.openCloseConfirmDialog(this)) {
		event->accept();
	} else {
		event->ignore();
	}
}
