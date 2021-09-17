#include  "fileselector.h"

#include <QFileDialog>
#include <QDir>
#include <QStandardPaths>

#include <cstring>
#include "nameutil.h"

FileSelector::FileSelector(QWidget* parent)
	:QObject(parent)
{}

void FileSelector::open(void)
{
	QString dir=configure.directory;
	if(!(configure.rememberLastDirectory && dir!="" && QDir(dir).exists())) {
		dir=QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
	}
	const QString selected=QFileDialog::getOpenFileName(static_cast<QWidget*>(parent()),
	                       "Open File",
	                       dir,
	                       "Images (*.png *.jpg *.bmp *.gif *.svg)");

	if(!selected.isNull()) {
		emit fileSelected(selected);

		if(configure.rememberLastDirectory) {
			configure.directory=extractDirectoryName(selected);
		}
		/*
		const char* dir_c_str=dir.toStdString().c_str();
		if(configure.rememberLastDirectory && std::strlen(dir_c_str)<512) {
			std::strcpy(configure.directory, dir_c_str);
		}*/
	}
}
