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
    src/QProcessErrorMsg.cpp \
    src/AVRDudeExecutor.cpp \
    src/AVRDudeConfParser.cpp \
    src/Part.cpp \
    src/Programmer.cpp \
    src/simppgmdlg.cpp

HEADERS  += \
    include/maindialog.h \
    include/QProcessErrorMsg.h \
    include/AVRDudeExecutor.h \
    include/delay.h \
    include/Part.h \
    include/Programmer.h \
    include/AVRDudeConfParser.h \
    include/ConfigParseException.h \
    include/simppgmdlg.h

FORMS    += \
    forms/maindialog.ui \
    forms/simppgmdlg.ui

INCLUDEPATH = include

include(qextserialport-1.2beta1/src/qextserialport.pri)

OBJECTS_DIR = output/obj
MOC_DIR = output/moc
DESTDIR = output

RESOURCES +=
