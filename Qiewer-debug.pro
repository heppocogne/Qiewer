QT += core gui widgets
TARGET = Qiewer
TEMPLATE = app

INCLUDEPATH += $$PWD $$PWD/include
CONFIG += debug

HEADERS += version.h mainwindow.h logger.h imageviewer.h
SOURCES += main.cpp version.cpp mainwindow.cpp logger.cpp imageviewer.cpp
