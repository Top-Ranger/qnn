#-------------------------------------------------
#
# Project created by QtCreator 2015-07-06T17:39:53
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = qnn
TEMPLATE = lib

DEFINES += QNN_LIBRARY
VERSION = 0.0.1

INCLUDEPATH += src/


SOURCES += \
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
    src/qnn-global.h
