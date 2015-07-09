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
    src/Networks/abstractneuralnetwork.cpp \
    src/Networks/feedforwardnetwork.cpp \
    src/genericsimulation.cpp \
    src/Networks/genericgene.cpp

HEADERS += \
    src/Networks/abstractneuralnetwork.h \
    src/genericsimulation.h \
    src/Networks/feedforwardnetwork.h \
    src/Networks/genericgene.h
