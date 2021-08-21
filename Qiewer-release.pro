QT += core gui widgets
TARGET = Qiewer
VERSION = 0.2.0
TEMPLATE = app

INCLUDEPATH += $$PWD $$PWD/include
CONFIG += release

RC_ICONS = eyecon.ico
HEADERS += version.h mainwindow.h logger.h imageviewer.h
SOURCES += main.cpp version.cpp mainwindow.cpp logger.cpp imageviewer.cpp
