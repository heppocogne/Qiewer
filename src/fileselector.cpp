#include  "fileselector.h"

#include <QFileDialog>
#include <QDir>
#include <QStandardPaths>

#include "nameutil.h"

FileSelector::FileSelector(QWidget* parent)
	:QObject(parent){}

void FileSelector::open(void)
{
	QString dir=configure.directory;
	if(!(configure.rememberLastDirectory && dir!="" && QDir(dir).exists())) {
		dir=QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
	}
	const QString selected=QFileDialog::getOpenFileName(reinterpret_cast<QWidget*>(parent()),
	                       "Open File",
	                       dir,
	                       "Images (*.png *.jpg *.bmp *.gif *.svg)");

	if(!selected.isNull()) {
		emit fileSelected(selected);

		if(configure.rememberLastDirectory) {
			configure.directory=extractDirectoryName(selected);
		}
	}
}
