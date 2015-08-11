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
    src/ga/genericgeneticalgorithm.cpp \
    src/network/continuoustimerecurrenneuralnetwork.cpp \
    src/ga/nonparallelgenericgeneticalgorithm.cpp \
    src/network/gasnet.cpp \
    src/network/lengthchanginggene.cpp \
    src/network/commonnetworkfunctions.cpp \
    src/network/modulatedspikingneuronsnetwork.cpp \
    src/simulation/tmazesimulation.cpp \
    src/network/networktoxml.cpp \
    src/simulation/rebergrammarsimulation.cpp

HEADERS += \
    src/network/abstractneuralnetwork.h \
    src/simulation/genericsimulation.h \
    src/network/feedforwardnetwork.h \
    src/network/genericgene.h \
    src/ga/genericgeneticalgorithm.h \
    src/qnn-global.h \
    src/network/continuoustimerecurrenneuralnetwork.h \
    src/ga/nonparallelgenericgeneticalgorithm.h \
    src/network/gasnet.h \
    src/network/lengthchanginggene.h \
    src/network/commonnetworkfunctions.h \
    src/network/modulatedspikingneuronsnetwork.h \
    src/simulation/tmazesimulation.h \
    src/network/networktoxml.h \
    src/simulation/rebergrammarsimulation.h

DESTDIR = $$PWD

OTHER_FILES += \
    LICENSE.LGPL3 \
    LICENSE.GPL3
