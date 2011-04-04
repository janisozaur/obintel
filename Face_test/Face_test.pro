#-------------------------------------------------
#
# Project created by QtCreator 2011-04-03T17:34:32
#
#-------------------------------------------------

QT       += core

#QT       -= gui

TARGET = Face_test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    configuration.cpp \
    cfgreader.cpp \
    imagescanner.cpp

HEADERS += \
    configuration.h \
    cfgreader.h \
    imagescanner.h

win32{
    INCLUDEPATH += C:/OI_face/include
    LIBS += -L C:/OI_face/libs -lfann
}
