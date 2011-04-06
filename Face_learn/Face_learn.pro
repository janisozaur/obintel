#-------------------------------------------------
#
# Project created by QtCreator 2011-04-02T13:50:02
#
#-------------------------------------------------

QT       += core

#QT       -= gui

TARGET = Face_learn
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    facesextractor.cpp \
    configuration.cpp \
	cfgreader.cpp


HEADERS += \
    facesextractor.h \
    configuration.h \
	cfgreader.h

LIBS += -lfann

win32{
	INCLUDEPATH += D:/Data/Study/OI/fann2.1.0/src/include
	LIBS += -L D:/Data/Study/OI/fann2.1.0/src/.libs
}
