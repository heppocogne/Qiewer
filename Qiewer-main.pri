QT += core gui widgets svg
TARGET = Qiewer
VERSION = 0.3.6
TEMPLATE = app

INCLUDEPATH += $$PWD/include

HEADERS +=	include/version.h	include/mainwindow.h	include/logger.h	include/imageviewer.h	include/configure.h	include/fileselector.h	include/nameutil.h	include/viewerif.h	include/svgviewer.h

SOURCES +=	src/main.cpp	src/version.cpp		src/mainwindow.cpp		src/logger.cpp		src/imageviewer.cpp		src/configure.cpp	src/fileselector.cpp	src/nameutil.cpp	src/viewerif.cpp	src/svgviewer.cpp

RESOURCES = rc/qiewer_resources.qrc
