QT += core gui widgets
TARGET = Qiewer
VERSION = 0.3.4
TEMPLATE = app

INCLUDEPATH += $$PWD/include

HEADERS +=					include/version.h	include/mainwindow.h	include/logger.h	include/imageviewer.h	include/configure.h	include/fileselector.h	include/nameutil.h
SOURCES +=	src/main.cpp	src/version.cpp		src/mainwindow.cpp		src/logger.cpp		src/imageviewer.cpp		src/configure.cpp	src/fileselector.cpp	src/nameutil.cpp
RESOURCES = rc/qiewer_resources.qrc
