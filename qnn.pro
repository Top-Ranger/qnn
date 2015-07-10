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
    src/network/abstractneuralnetwork.cpp \
    src/network/feedforwardnetwork.cpp \
    src/simulation/genericsimulation.cpp \
    src/network/genericgene.cpp \
    src/ga/genericgeneticalgorithm.cpp

HEADERS += \
    src/network/abstractneuralnetwork.h \
    src/simulation/genericsimulation.h \
    src/network/feedforwardnetwork.h \
    src/network/genericgene.h \
    src/ga/genericgeneticalgorithm.h \
    src/ConsoleOutput.h
