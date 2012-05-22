#-------------------------------------------------
#
# Project created by QtCreator 2012-05-18T18:36:47
#
#-------------------------------------------------

QT       += core gui

TARGET = TMFAVRCalculator
TEMPLATE = app


SOURCES += \
    src/maindialog.cpp \
    src/main.cpp \
    src/programmer.cpp \
    src/part.cpp \
    src/avrdudeconfparser.cpp

HEADERS  += \
    include/maindialog.h \
    include/programmer.h \
    include/part.h \
    include/avrdudeconfparser.h

FORMS    += \
    forms/maindialog.ui

INCLUDEPATH = include

OBJECTS_DIR = output/obj
MOC_DIR = output/moc
DESTDIR = output
