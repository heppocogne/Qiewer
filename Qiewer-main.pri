QT += core gui widgets
TARGET = Qiewer
VERSION = 0.2.2
TEMPLATE = app

INCLUDEPATH += $$PWD $$PWD/include

HEADERS += version.h mainwindow.h logger.h imageviewer.h configure.h
SOURCES += main.cpp version.cpp mainwindow.cpp logger.cpp imageviewer.cpp configure.cpp

