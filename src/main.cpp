#include <QApplication>
#include <QtGlobal>
#include <QSharedMemory>
#include <QString>
#include <QMessageBox>
#include <QStandardPaths>
#include <QTextCodec>

#include "version.h"
#include "mainwindow.h"
#include "configure.h"
#include "nameutil.h"
#include "logger.h"
#include <cstring>
#include <thread>
#include <chrono>


Logger logger;
ConfigureIO configureIO;
const Version version(0, 3, 4, "alpha");


int main(int argc, char* argv[])
{
	if(argc<=1) {
		return 0;
	}
	QApplication app(argc, argv);

	QSharedMemory input(MainWindow::sharedMemoryKey);
	input.attach();

	int exitCode;

	if(input.isAttached()) {	//there's already Qiewer process
		if(MainWindow::sharedMemorySize<=std::strlen(argv[1])) {
			QMessageBox::warning(nullptr, "Warning: Too long file name", "File name limitation is 1023 bytes");

			app.exit(1);
		}

		const auto timeout=std::chrono::seconds(10);
		const auto retryInterval=std::chrono::milliseconds(500);

		exitCode=1;
		for(auto t=std::chrono::milliseconds(0); t<timeout; t+=retryInterval) {
			if(input.lock()) {
				strcpy(reinterpret_cast<char*>(input.data()), argv[1]);
				input.unlock();
				exitCode=0;
				break;
			}
			std::this_thread::sleep_for(retryInterval);
		}

		app.exit(1);
		return exitCode;	//unreachable code
	} else {
		const QString directoryName=
#ifdef QT_DEBUG
		    extractDirectoryName(QString::fromLocal8Bit(argv[0]));
#else
		    QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
#endif

		logger.openFile(directoryName+"\\qiewer.log");
		configureIO.load(directoryName+"\\.qiewerconfig");

		logger.write("Qiewer version:	"+(QString)version, LOG_FROM);
		logger.write("QuickTime version:	"+QString(qVersion()), LOG_FROM);



		MainWindow* const window=new MainWindow();
		const QString imageFileName=QString::fromLocal8Bit(argv[1]);
		if(window->addImage(imageFileName)) {
			window->showProperly();
			exitCode=app.exec();
		} else {
			app.exit(1);
			exitCode=1;	//unreachable code
		}

		configureIO.save();

		delete window;
		return exitCode;
	}
}
