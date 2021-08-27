#include <QApplication>
#include <QtGlobal>
#include <QSharedMemory>

#include "version.h"
#include "mainwindow.h"
#include "configure.h"
#include "nameutil.h"
#include "logger.h"
#include <QString>
#include <cstring>
#include <QMessageBox>
#include <thread>
#include <chrono>


Logger logger;
ConfigureIO configureIO;
const Version version(0, 3, 1, "alpha");


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
		logger.openFile(extractDirectoryName(argv[0])+"\\qiewer.log");
		logger.write("Qiewer version:	"+(QString)version, LOG_FROM);
		logger.write("QuickTime version:	"+QString(qVersion()), LOG_FROM);

		configureIO.load(extractDirectoryName(argv[0])+"\\.qiewerconfig");

		MainWindow* const window=new MainWindow();
		if(window->addImage(argv[1])) {
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
