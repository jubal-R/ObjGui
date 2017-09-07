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
    highlighters/disassemblyhighlighter.cpp \
    highlighters/headerhighlighter.cpp \
    resultsdialog.cpp \
    stringsdumper.cpp \
    disassemblycore.cpp \
    model/function.cpp \
    model/section.cpp \
    model/strings.cpp

HEADERS  += mainwindow.h \
    files.h \
    objdumper.h \
    highlighters/disassemblyhighlighter.h \
    highlighters/headerhighlighter.h \
    resultsdialog.h \
    stringsdumper.h \
    disassemblycore.h \
    model/function.h \
    model/section.h \
    model/strings.h

FORMS    += mainwindow.ui \
    resultsdialog.ui \
    optionsdialog.ui

RESOURCES += \
    resources.qrc

DISTFILES += \
    fonts/Anonymous Pro B.ttf \
    fonts/Anonymous Pro BI.ttf \
    fonts/Anonymous Pro I.ttf
