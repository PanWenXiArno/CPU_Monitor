TEMPLATE = app
CONFIG += console c++11
CONFIG += app_bundle
CONFIG += qt

QT += core gui widgets

SOURCES += \
        main.cpp \
        processMonitor.cpp

HEADERS += \
    processMonitor.h
