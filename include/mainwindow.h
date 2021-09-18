#ifndef MAINWINDOW_H_INCLUDED
#define MAINWINDOW_H_INCLUDED

#include <QMainWindow>
#include <QWidget>
#include <QTabWidget>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QEvent>
#include <QSharedMemory>
#include <QTimer>
#include <QToolBar>

#include "configure.h"
#include "fileselector.h"
#include "arraypipe.h"

class ViewerInterface;

class MainWindow: public QMainWindow
{
		Q_OBJECT
		
		QTabWidget* const viewertabs;
		QToolBar* const toolbar;
		
		FileSelector* const fileSelector;

		ArrayPipe arrayPipe;
		QTimer* const sharedMemoryTick;
		QTimer* const cursorTick;
		
		ViewerInterface* currentView(void)const;
		int addImageMain(const QString& imageFileName);
		void addImagePostProcess(int idx);
public:
		constexpr static const char* sharedMemoryPrefix="Qiewer/image/input/";
		
		MainWindow(QWidget* parent=nullptr);
		virtual ~MainWindow();
		void showProperly(void);
		

	public slots:
		void closeTab(int idx);
		void checkSharedMemory(void);
		void checkMousePosition(void);
		bool addImage(const QString& imageFileName);
		bool addImages(const QStringList& imageFileList);
		void viewerCloseRequested(ViewerInterface* viewer);
		void reload(void);
		void fitSize(void);
		void actualSize(void);
		void zoom(int value);
		//void setting(void);

	protected:
		virtual void resizeEvent(QResizeEvent *event);
		virtual void changeEvent(QEvent* event);
		virtual void dragEnterEvent(QDragEnterEvent* event);
		virtual void dropEvent(QDropEvent* event);
		virtual void closeEvent(QCloseEvent *event);
};

#endif
