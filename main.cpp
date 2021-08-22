#include <QApplication>
#include <QtGlobal>
#include <QSharedMemory>

#include "version.h"
#include "mainwindow.h"
#include "logger.h"
#include <string>


std::string extractDirectoryName(const std::string& fileName);
Logger logger;
const Version version(0, 2, 4, "alpha");


int main(int argc, char* argv[])
{
	if(argc<=1) {
		return 0;
	}
	QApplication app(argc, argv);

	QSharedMemory input(MainWindow::sharedMemoryKey);
	input.attach();

	if(input.isAttached()) {	//there's already Qiewer process
		input.lock();
		strcpy(reinterpret_cast<char*>(input.data()), argv[1]);
		input.unlock();

		app.exit(0);
		
		return 0;	//unreachable code
	} else {
		logger.openFile(extractDirectoryName(argv[0])+"\\qiewer.log");
		
		logger.write("Qiewer version:	"+(std::string)version , LOG_FROM);
		logger.write("QuickTime version:	"+(std::string)qVersion(), LOG_FROM);
		
		const std::string imageFile=std::string(argv[1]);

		MainWindow* const window=new MainWindow(extractDirectoryName(argv[0])+"\\.qiewerconfig");
		window->addImage(imageFile);
		window->showProperly();

		const int exitCode=app.exec();

		//window->saveConfig();
		delete window;
		
		return exitCode;
	}
}


std::string extractDirectoryName(const std::string& fileName)
{
	int idx=fileName.length()-1;
	for(; 0<=idx; idx--) {
		if(fileName[idx]=='\\'||fileName[idx]=='/') {
			break;
		}
	}
	//idx=-1 or fileName[idx]=\ or /
	return fileName.substr(0,idx);
}