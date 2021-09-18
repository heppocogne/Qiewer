#include <QApplication>
#include <QtGlobal>
#include <QStringList>
#include <QString>
#include <QMessageBox>
#include <QStandardPaths>

#include "version.h"
#include "mainwindow.h"
#include "configure.h"
#include "nameutil.h"
#include "arraypipe.h"
#include "logger.h"
#include "winmutex.h"
#include <windows.h>
#include <cstring>
#include <thread>
#include <chrono>
//#include <boost/interprocess/sync/named_mutex.hpp>
//#include <boost/interprocess/exceptions.hpp>


Logger logger;
Configure configure;
const Version version(0, 3, 6, "alpha");


int main(int argc, char* argv[])
{
	if(argc<=1) {
		return 0;
	}
	QApplication app(argc, argv);

	QStringList fileList;
	for(int i=1; i<argc; i++) {
		if(ArrayPipe::bufferSize<=std::strlen(argv[i])) {
			QMessageBox::warning(nullptr, "Warning: Too long file name", "File name limitation is "+QString::number(ArrayPipe::bufferSize-1)+" bytes");
			app.exit(1);
		}
		fileList<<argv[i];
	}

	int exitCode=1;
	WinMutex qiewerMutex(L"qiewer_mutex");

	if(qiewerMutex.getState()==WinMutex::State::Created) {	//new process
		const QString directoryName=
#ifdef QT_DEBUG
		    extractDirectoryName(QString::fromLocal8Bit(argv[0]));
#else
		    QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
#endif
		logger.openFile(directoryName+"\\qiewer.log");
		configure.load(directoryName+"\\.qiewerconfig");

		logger.write("Qiewer version:	"+(QString)version, LOG_FROM);
		logger.write("QuickTime version:	"+QString(qVersion()), LOG_FROM);

		MainWindow* const window=new MainWindow();

		if(window->addImages(fileList)) {
			window->showProperly();
			exitCode=app.exec();
		} else {
			exitCode=1;
		}
		configure.save();
		delete window;

		qiewerMutex.close();

	} else if(qiewerMutex.getState()==WinMutex::State::AlreadyExists) {	//there's already Qiewer process
		//wait for the window visible
		for(int i=0; FindWindow(nullptr, L"Qiewer")==nullptr&&i<50; i++)
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		
		ArrayPipe input(MainWindow::sharedMemoryPrefix);
		input.attach();
		//wait if there are already parameters
		while(input.currentSize()!=0)
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		
		input.paste(fileList);
		//wait until the parameters have been read
		while(input.currentSize()!=0)
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		
		exitCode=0;
	} else {
		exitCode=1;
	}

	return exitCode;
}



int main2(int argc, char* argv[])
{
	if(argc<=1) {
		return 0;
	}
	QApplication app(argc, argv);

	QStringList fileList;
	for(int i=1; i<argc; i++) {
		if(ArrayPipe::bufferSize<=std::strlen(argv[i])) {
			QMessageBox::warning(nullptr, "Warning: Too long file name", "File name limitation is "+QString::number(ArrayPipe::bufferSize-1)+" bytes");
			app.exit(1);
		}
		fileList<<argv[i];
	}

	int exitCode=1;
	ArrayPipe input(MainWindow::sharedMemoryPrefix);
	if(input.attach()) {	//there's already Qiewer process
		input.paste(fileList);
		while(input.currentSize()!=0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		exitCode=0;
	} else {	//new process

		const QString directoryName=
#ifdef QT_DEBUG
		    extractDirectoryName(QString::fromLocal8Bit(argv[0]));
#else
		    QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
#endif
		logger.openFile(directoryName+"\\qiewer.log");
		configure.load(directoryName+"\\.qiewerconfig");

		logger.write("Qiewer version:	"+(QString)version, LOG_FROM);
		logger.write("QuickTime version:	"+QString(qVersion()), LOG_FROM);

		MainWindow* const window=new MainWindow();

		if(window->addImages(fileList)) {
			window->showProperly();
			exitCode=app.exec();
		} else {
			app.exit(1);
		}
		configure.save();
		delete window;
	}
	return exitCode;
}
