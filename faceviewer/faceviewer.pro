#-------------------------------------------------
#
# Project created by QtCreator 2011-03-31T20:20:02
#
#-------------------------------------------------

QT       += core gui

TARGET = faceviewer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    paintarea.cpp

HEADERS  += mainwindow.h \
    paintarea.h

FORMS    += mainwindow.ui

RESOURCES += \
	resources.qrc

*-g++ {
	QMAKE_CXXFLAGS += -O2
#	QMAKE_CXXFLAGS += -std=c++0x
}

CONFIG(debug, debug|release) {
	CONFIG += console
	DEFINES += __DEBUG__ \
		__debug__ \
		DEBUG
}

CODECFORTR = UTF-8
CODECFORSRC = UTF-8
