QT += core gui widgets
TARGET = Qiewer
TEMPLATE = app

INCLUDEPATH += $$PWD $$PWD/include
CONFIG += release

RC_ICONS = eyecon.ico
HEADERS += version.h mainwindow.h logger.h imageviewer.h
SOURCES += main.cpp version.cpp mainwindow.cpp logger.cpp imageviewer.cpp
