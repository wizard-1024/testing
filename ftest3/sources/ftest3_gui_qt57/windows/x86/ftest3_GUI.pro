#-------------------------------------------------
#
# Project created by QtCreator 2016-07-09T10:48:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ftest3_GUI
TEMPLATE = app

CONFIG   += console

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH += .

#RC_ICONS = ftest3_GUI.ico
RC_FILE = ftest3_GUI.rc
RESOURCES += ftest3_GUI.qrc

