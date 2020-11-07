#-------------------------------------------------
#
# Project created by QtCreator 2020-09-04T12:38:29
#
#-------------------------------------------------

QT       += core network gui widgets
#QT+=widgets

#QT       -= gui

TARGET = qlib
CONFIG   += console
#CONFIG   -= app_bundle
CONFIG +=c++11
TEMPLATE = app


SOURCES += \
    qlib.cpp \
    mainqlib.cpp \
    plib/plib.cpp \
    plib/pxml.cpp \
    plib/tinystr.cpp \
    plib/tinyxml.cpp \
    plib/tinyxmlerror.cpp \
    plib/tinyxmlparser.cpp \
    plib/ptrans.cpp

HEADERS += \
    qlib.h \
    plib/plib.h \
    plib/pxml.h \
    plib/tinystr.h \
    plib/tinyxml.h \
    plib/ptrans.h
