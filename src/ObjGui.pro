#-------------------------------------------------
#
# Project created by QtCreator 2017-03-25T19:08:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ObjGui
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    files.cpp \
    objdumper.cpp \
    highlighter.cpp \
    settings.cpp

HEADERS  += mainwindow.h \
    files.h \
    objdumper.h \
    highlighter.h \
    settings.h

FORMS    += mainwindow.ui
