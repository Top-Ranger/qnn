#-------------------------------------------------
#
# Project created by QtCreator 2015-07-06T17:39:53
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = qnn
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += src/


SOURCES += src/main.cpp \
    src/abstractneuralnetwork.cpp \
    src/generalgene.cpp \
    src/generalsimulation.cpp

HEADERS += \
    src/abstractneuralnetwork.h \
    src/generalgene.h \
    src/genericsimulation.h
