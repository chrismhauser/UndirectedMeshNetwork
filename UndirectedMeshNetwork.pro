#-------------------------------------------------
#
# Project created by QtCreator 2015-03-26T13:03:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UndirectedMeshNetwork
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mesh.cpp \
    ip.cpp \
    itoa.cpp

HEADERS  += mainwindow.h \
    mesh.h \
    ip.h

FORMS    += mainwindow.ui

CONFIG += c++11
