#ifndef MAINWINDOW_H_INCLUDED
#define MAINWINDOW_H_INCLUDED

#include <string>
#include <QImageReader>
#include <QMainWindow>
#include <QWidget>
#include <QTabWidget>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QEvent>
#include <fstream>
#include <QSharedMemory>
#include <QTimer>
#include "configure.h"
#include <QToolBar>
#include "nameutil.h"
#include "fileselector.h"

class MainWindow: public QMainWindow
{
		Q_OBJECT
		QImageReader* const imageReader;
		QTabWidget* const viewertabs;
		QToolBar* const toolbar;
		
		FileSelector* const fileSelector;

		QSharedMemory* const sharedMemory;
		QTimer* const sharedMemoryTick;
		QTimer* const cursorTick;
	public:
		/*
				static const char* sharedMemoryKey;
				static const size_t sharedMemorySize;
		*/
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

	protected:
		virtual void resizeEvent(QResizeEvent *event);
		virtual void changeEvent(QEvent* event);
		virtual void dragEnterEvent(QDragEnterEvent* event);
		virtual void dropEvent(QDropEvent* event);
};

#endif
