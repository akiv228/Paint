QT += widgets
TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(trapezoidexplugin)
DESTDIR = ../plugins

HEADERS += \
    trapezoidex.h \
    trapezoidexplugin.h

SOURCES += \
    trapezoidex.cpp \
    trapezoidexplugin.cpp

INCLUDEPATH += ../Paint
