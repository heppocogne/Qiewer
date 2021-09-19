#ifndef MAINWINDOW_H_INCLUDED
#define MAINWINDOW_H_INCLUDED

#include <QMainWindow>
#include <QWidget>
#include <QTabWidget>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QEvent>
#include <QTimer>
#include <QToolBar>
#include <QLocalServer>

#include "configure.h"
#include "fileselector.h"

class ViewerInterface;

class MainWindow: public QMainWindow
{
		Q_OBJECT
		
		QTabWidget* const viewertabs;
		QToolBar* const toolbar;
		
		FileSelector* const fileSelector;

		QTimer* const cursorTick;
		
		QLocalServer* const server;
		
		ViewerInterface* currentView(void)const;
		int addImageMain(const QString& imageFileName);
		void addImagePostProcess(int idx);
public:
		constexpr static const char* serverName="qiewer.image.input";
		
		MainWindow(QWidget* parent=nullptr);
		virtual ~MainWindow();
		void showProperly(void);

	public slots:
		void closeTab(int idx);
		void checkMousePosition(void);
		bool addImage(const QString& imageFileName);
		bool addImages(const QStringList& imageFileList);
		void viewerCloseRequested(ViewerInterface* viewer);
		void reload(void);
		void fitSize(void);
		void actualSize(void);
		void zoom(double value);
		//void setting(void);
		void processConnection(void);

	protected:
		virtual void resizeEvent(QResizeEvent *event);
		virtual void changeEvent(QEvent* event);
		virtual void dragEnterEvent(QDragEnterEvent* event);
		virtual void dropEvent(QDropEvent* event);
		virtual void closeEvent(QCloseEvent *event);
};

#endif
