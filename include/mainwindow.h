#ifndef MAINWINDOW_H_INCLUDED
#define MAINWINDOW_H_INCLUDED

#include <QMainWindow>
#include <QWidget>
#include <QTabWidget>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QEvent>
#include <QSharedMemory>
#include <QTimer>
#include <QToolBar>

#include "configure.h"
#include "fileselector.h"

class ViewerInterface;

class MainWindow: public QMainWindow
{
		Q_OBJECT
		QTabWidget* const viewertabs;
		QToolBar* const toolbar;
		
		FileSelector* const fileSelector;

		QSharedMemory* const sharedMemory;
		QTimer* const sharedMemoryTick;
		QTimer* const cursorTick;
		
		ViewerInterface* currentView(void)const;
	public:
		constexpr static const char* sharedMemoryKey="Qiewer/image/input";
		constexpr static const size_t sharedMemorySize=1024;
		MainWindow();
		virtual ~MainWindow();
		void showProperly(void);
		

	public slots:
		void closeTab(int idx);
		void checkSharedMemory(void);
		void checkMousePosition(void);
		bool addImage(const QString& imageFileName);
		void reload(void);
		void fitSize(void);
		void actualSize(void);
		void zoomin(void);
		void zoomout(void);
		void zoom(int value);
		//void setting(void);

	protected:
		virtual void resizeEvent(QResizeEvent *event);
		virtual void changeEvent(QEvent* event);
		virtual void dragEnterEvent(QDragEnterEvent* event);
		virtual void dropEvent(QDropEvent* event);
};

#endif
