#-------------------------------------------------
#
# Project created by QtCreator 2011-06-07T18:07:12
#
#-------------------------------------------------

QT       += core

#QT       -= gui

TARGET = FaceDetector
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    parameterreader.cpp \
    utils.cpp \
    configuration.cpp

HEADERS += \
    parameterreader.h \
    utils.h \
    configuration.h


win32{
        INCLUDEPATH += D:/Data/Study/OI/fann2.1.0/src/include
		LIBS += -L D:/Data/Study/OI/fann2.1.0/src/.libs
}
unix {
	LIBS += -L/usr/local/lib
}

LIBS += -lfann
