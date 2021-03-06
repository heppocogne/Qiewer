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
#include <QLocalSocket>
#include <QByteArray>
#include <QIODevice>

#include "imageviewer.h"
#include "svgviewer.h"
#include "nameutil.h"
#include "logger.h"
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif


MainWindow::MainWindow(QWidget* parent)
	:QMainWindow(parent, Qt::Window),
	 viewertabs(new QTabWidget(this)),
	 toolbar(new QToolBar(this)),
	 fileSelector(new FileSelector(this)),
	 cursorTick(new QTimer(this)),
	 server(new QLocalServer(this))
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
	toolbar->addAction(QIcon(":/rc/zoomin24.png"), "Zoom In", [&] {zoom(1);});

	//setup zooout icon
	toolbar->addAction(QIcon(":/rc/zoomout24.png"), "Zoom Out",  [&] {zoom(-1);});

	//setup view actual size icon
	toolbar->addAction(QIcon(":/rc/100%24.png"), "Actual Size", this, &MainWindow::actualSize);

	//setup fit size icon
	toolbar->addAction(QIcon(":/rc/fit24.png"), "Fit to Window", this, &MainWindow::fitSize);

	//setup setting icon
	toolbar->addSeparator();
	toolbar->addAction(QIcon(":/rc/gear24.png"), "Setting", [&] {configure.openConfigureDialog();});


	//setup server
	server->listen(serverName);
	connect(server, &QLocalServer::newConnection, this, &MainWindow::processConnection);


	//connect signals
	connect(viewertabs, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
	connect(cursorTick, &QTimer::timeout, this, &MainWindow::checkMousePosition);


	//check mouse position every 0.05s
	cursorTick->start(std::chrono::milliseconds(50));
}


MainWindow::~MainWindow()
{
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


int MainWindow::addImageMain(const QString& imageFileName)
{
	logger.write("add image:	"+imageFileName, LOG_FROM);

	int idx;
	for(idx=0; idx<viewertabs->count(); idx++) {
		if(static_cast<ViewerInterface*>(viewertabs->widget(idx))->getFileName()==imageFileName) {
			break;
		}
	}

	if(configure.allowDuplicatedFiles || viewertabs->count()<=idx) {
		const auto imageFormat=QImageReader::imageFormat(imageFileName);

		logger.write("format="+QString(imageFormat), LOG_FROM);

		ViewerInterface* viewer;
		if(imageFormat=="svg" || imageFormat=="svgz") {
			viewer=new SvgViewer(viewertabs);
		} else {
			viewer=new ImageViewer(viewertabs);
		}

		if(viewer->setImageFile(imageFileName)) {
			connect(viewer, &ViewerInterface::closeMe, this, &MainWindow::viewerCloseRequested);
			return viewertabs->addTab(viewer, extractFileName(imageFileName));
		} else {
			//delete viewer;
			viewer->setParent(nullptr);
			delete viewer;
			return -1;
		}
	} else {
		return idx;
	}
}


void MainWindow::addImagePostProcess(int idx)
{
	viewertabs->setCurrentIndex(idx);

	setWindowState(windowState()&~Qt::WindowMinimized);
	if(maximized) {
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
}


bool MainWindow::addImage(const QString& imageFileName)
{
	const int idx=addImageMain(imageFileName);

	if(idx<0)
		return false;

	addImagePostProcess(idx);
	return true;
}


bool MainWindow::addImages(const QStringList& imageFileList)
{
	int idx=-1;
	for(QString imageFileName: imageFileList) {
		const int newIdx=addImageMain(imageFileName);
		if(idx<0)
			idx=newIdx;
	}
	if(idx<0)
		return false;

	addImagePostProcess(idx);
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

void MainWindow::zoom(double value)
{
	ViewerInterface* const viewer=currentView();
	if(viewer) {
		viewer->zoom(value);
	}
}


void MainWindow::showProperly(void)
{
	if(configure.windowSizeMode==Configure::ALWAYS_MAXIMIZED || (configure.windowSizeMode==Configure::REMEMBER_SIZE && configure.maximized)) {
		logger.write("show maximized window", LOG_FROM);
		maximized=true;
		showMaximized();
	} else {
		logger.write("show normal window:	"+QString::number(configure.windowWidth)+"x"+QString::number(configure.windowHeight), LOG_FROM);

		const auto& available=QGuiApplication::primaryScreen()->availableSize();
		setGeometry((available.width()-configure.windowWidth)/2, (available.height()-configure.windowHeight)/2,
		            configure.windowWidth, configure.windowHeight);
		maximized=false;
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
}


void MainWindow::changeEvent(QEvent* event)
{
	switch(event->type()) {
		case  QEvent::WindowStateChange:
			maximized=bool(windowState()&Qt::WindowMaximized);
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
	for(const auto& url: event->mimeData()->urls()) {
		addImage(url.toLocalFile().replace('/', '\\'));
	}
}


void MainWindow::closeEvent(QCloseEvent *event)
{
	if((!configure.confirmBeforeQuit) || viewertabs->count()<=1 || configure.openCloseConfirmDialog()) {
		if(configure.windowSizeMode==Configure::REMEMBER_SIZE) {
			configure.windowWidth=width();
			configure.windowHeight=height();
			if(windowState()&Qt::WindowMaximized){
				configure.maximized=bool(windowState()&Qt::WindowMaximized);
			}
		}
		event->accept();
	} else {
		event->ignore();
	}
}


void MainWindow::processConnection(void)
{
	QLocalSocket*const socket=server->nextPendingConnection();
	connect(socket, &QLocalSocket::disconnected, socket, &QLocalSocket::deleteLater);
	socket->waitForReadyRead();

	QByteArray bytes=socket->readAll();
	QDataStream input(&bytes, QIODevice::ReadOnly);
	input.setVersion(QDataStream::Qt_5_15);
	QStringList files;
	while(!input.atEnd()) {
		QByteArray data;
		input>>data;
		const QString file=QString::fromLocal8Bit(data);
		logger.write("read from socket:"+file, LOG_FROM);
		files<<file;
	}
	addImages(files);

	socket->disconnectFromServer();
}
