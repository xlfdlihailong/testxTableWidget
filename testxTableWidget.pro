#-------------------------------------------------
#
# Project created by QtCreator 2020-09-02T22:49:30
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = testxTableWidget
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ../../cpp/plib/plib.cpp \
    ../qlib/qlib.cpp \
    xTableWidget.cpp

HEADERS  += mainwindow.h \
    ../../cpp/plib/plib.h \
    ../qlib/qlib.h \
    xTableWidget.h

FORMS    += mainwindow.ui \
    xTableWidget.ui

RESOURCES += \
    xlfd.qrc
