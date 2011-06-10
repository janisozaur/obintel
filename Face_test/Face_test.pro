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
	INCLUDEPATH += D:/Data/Study/OI/fann2.1.0/src/include
	LIBS += -L D:/Data/Study/OI/fann2.1.0/src/.libs -lfann
}
