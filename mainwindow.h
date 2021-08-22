#ifndef MAINWINDOW_H_INCLUDED
#define MAINWINDOW_H_INCLUDED

#include <string>
#include <QImageReader>
#include <QMainWindow>
#include <QWidget>
#include <QTabWidget>
#include <QResizeEvent>
#include <QEvent>
#include <fstream>
#include <QSharedMemory>
#include <QTimer>
#include "configure.h"


class MainWindow: public QMainWindow
{
		Q_OBJECT
		QImageReader* const imageReader;
		QTabWidget* const viewertabs;
		static std::string extractFileName(const std::string& fileName);

		QSharedMemory* const sharedMemory;
		QTimer* const timer;
		
		ConfigureIO configureIO;
		//void saveConfig(void);
		bool firstImage;
	public:
		static const char* sharedMemoryKey;
		static const size_t sharedMemorySize;

		MainWindow(const std::string& configFile);
		virtual ~MainWindow();
		void addImage(const std::string& imageFileName);
		void showProperly(void);

	public slots:
		void closeTab(int idx);
		void checkSharedMemory(void);

	protected:
		virtual void resizeEvent(QResizeEvent *event);
		virtual void changeEvent(QEvent* event);
		virtual void dragEnterEvent(QDragEnterEvent *event);
		virtual void dropEvent(QDropEvent *event);
};

#endif
