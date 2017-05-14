#-------------------------------------------------
#
# Project created by QtCreator 2017-01-20T20:58:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ELFviewer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mygraphicalview.cpp \
    machine_os.cpp

HEADERS  += mainwindow.h \
    myqgraphicsview.h \
    elf.h \
    machine_os.h

FORMS    += mainwindow.ui
