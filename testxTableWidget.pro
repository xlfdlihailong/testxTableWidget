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
    qlib/qlib.cpp \
    qlib/plib/plib.cpp \
    qlib/plib/pxml.cpp \
    qlib/plib/tinystr.cpp \
    qlib/plib/tinyxml.cpp \
    qlib/plib/tinyxmlerror.cpp \
    qlib/plib/tinyxmlparser.cpp \
    xtablewidget.cpp \
    xtreewidget.cpp \
    qtreewidgetexx.cpp \
    xfilebrowserlocal.cpp

HEADERS  += mainwindow.h \
    qlib/qlib.h \
    qlib/plib/plib.h \
    qlib/plib/pxml.h \
    qlib/plib/tinystr.h \
    qlib/plib/tinyxml.h \
    xtablewidget.h \
    xtreewidget.h \
    qtreewidgetexx.h \
    xfilebrowserlocal.h

FORMS    += mainwindow.ui \
    xtablewidget.ui \
    xtreewidget.ui \
    qtreewidgetexx.ui \
    xfilebrowserlocal.ui

RESOURCES += \
    xlfd.qrc
