TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    plib.cpp \
    pxml.cpp \
    tinystr.cpp \
    tinyxml.cpp \
    tinyxmlerror.cpp \
    tinyxmlparser.cpp \
    mainplib.cpp \
    pudp.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    plib.h \
    pxml.h \
    tinystr.h \
    tinyxml.h \
    pudp.h

