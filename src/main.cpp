#include <QApplication>
#include <QtGlobal>
#include <QStringList>
#include <QString>
#include <QMessageBox>
#include <QStandardPaths>
#include <QLocalSocket>
#include <QDataStream>
#include <QByteArray>
#include <QIODevice>

#include "version.h"
#include "mainwindow.h"
#include "configure.h"
#include "nameutil.h"
#include "logger.h"
#include "winmutex.h"
#include <windows.h>
#include <thread>
#include <chrono>


Logger logger;
Configure configure;
const Version version(0, 3, 6, "alpha");


int main(int argc, char* argv[])
{
	if(argc<=1) {
		return 0;
	}
	QApplication app(argc, argv);

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

		QStringList fileList;
		for(int i=1; i<argc; i++) {
			fileList<<QString::fromLocal8Bit(argv[i]);
		}

		MainWindow* const window=new MainWindow();

		if(window->addImages(fileList)) {
			window->showProperly();
			exitCode=app.exec();
		} else {
			exitCode=1;
		}
		configure.save();
		delete window;

	} else if(qiewerMutex.getState()==WinMutex::State::AlreadyExists) {	//there's already Qiewer process
		//wait for the window visible
		for(int i=0; FindWindow(nullptr, L"Qiewer")==nullptr&&i<50; i++)
			std::this_thread::sleep_for(std::chrono::milliseconds(100));

		QLocalSocket socket;
		socket.connectToServer(MainWindow::serverName);
		socket.waitForConnected();
		
		QByteArray bytes;
		QDataStream output(&bytes, QIODevice::ReadWrite);
		output.setVersion(QDataStream::Qt_5_15);
		for(int i=1; i<argc; i++) {
			output<<argv[i];
		}
		socket.write(bytes);
		socket.flush();
		socket.waitForBytesWritten();

		exitCode=0;
	} else {
		exitCode=1;
	}

	return exitCode;
}
