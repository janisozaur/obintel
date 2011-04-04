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

win32{
    INCLUDEPATH += C:/OI_face/include

    LIBS += -L C:/OI_face/libs -lfann
}
