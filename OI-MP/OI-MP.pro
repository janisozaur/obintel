#-------------------------------------------------
#
# Project created by QtCreator 2011-05-11T17:05:06
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = OI-MP
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    microarray.cpp \
    kmeans.cpp \
    euclidean.cpp \
    pearson.cpp \
    spearman.cpp \
    utils.cpp \
    configuration.cpp \
    parameterreader.cpp

HEADERS += \
    microarray.h \
    kmeans.h \
    euclidean.h \
    distance.h \
    pearson.h \
    spearman.h \
    utils.h \
    configuration.h \
    parameterreader.h

win32{
        INCLUDEPATH += D:/Data/Study/OI/fann2.1.0/src/include
        LIBS += -L D:/Data/Study/OI/fann2.1.0/src/.libs -lfann
}
