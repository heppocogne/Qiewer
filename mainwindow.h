#ifndef MAINWINDOW_H_INCLUDED
#define MAINWINDOW_H_INCLUDED

#include <string>
#include <QImageReader>
#include <QMainWindow>
#include <QWidget>
#include <QTabWidget>
#include <QResizeEvent>
#include <fstream>
#include <QSharedMemory>
#include <QTimer>


class MainWindow: public QMainWindow
{
		Q_OBJECT
		QImageReader* const imageReader;
		QTabWidget* const viewertabs;
		static std::string extractFileName(const std::string& fileName);

		struct WindowConfigure {
			int width, height;
		} config;

		QSharedMemory* const sharedMemory;
		QTimer* const timer;

	public:
		static const char* sharedMemoryKey;
		static const size_t sharedMemorySize;

		MainWindow(const std::string& configFile);
		virtual ~MainWindow();
		void addImage(const std::string& imageFileName);
		void showProperly(void);
		void outputConfig(const std::string& configFile);

	public slots:
		void closeTab(int idx);
		void checkSharedMemory(void);

	protected:
		virtual void resizeEvent(QResizeEvent *event);
};

#endif
