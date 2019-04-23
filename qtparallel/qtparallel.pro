#-------------------------------------------------
#
# Project created by QtCreator 2014-11-19T21:48:29
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = qtparallel
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    processclass.cpp \
    parallelclass.cpp \
    ../interface/esy_argument.cc \
    ../interface/esy_conv.cc \
    ../tinyxml/tinystr.cpp \
    ../tinyxml/tinyxml.cpp \
    ../tinyxml/tinyxmlparser.cpp \
    ../tinyxml/tinyxmlerror.cpp \

HEADERS += \
    processclass.h \
    parallelclass.h \
    ../interface/esy_argument.h \
    ../interface/esy_conv.h \
    ../tinyxml/tinystr.h \
    ../tinyxml/tinyxml.h \

INCLUDEPATH += ../interface
INCLUDEPATH += ../tinyxml

MOC_DIR = ./moc
OBJECTS_DIR = ./obj
DESTDIR = ./
