#-------------------------------------------------
#
# Project created by QtCreator 2012-05-18T18:36:47
#
#-------------------------------------------------

QT       += core gui xml xmlpatterns

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
    src/simppgmdlg.cpp \
    src/AVRDudeErrorWindow.cpp \
    src/ELFProcessor.cpp \
    src/Value.cpp \
    src/Bit.cpp \
    src/AVRStudioXMLParser.cpp \
    src/AVRFactory.cpp \
    src/ProgrammingInterface.cpp

HEADERS  += \
    include/maindialog.h \
    include/QProcessErrorMsg.h \
    include/AVRDudeExecutor.h \
    include/delay.h \
    include/Part.h \
    include/Programmer.h \
    include/AVRDudeConfParser.h \
    include/ConfigParseException.h \
    include/simppgmdlg.h \
    include/AVRDudeErrorWindow.h \
    include/ELFProcessor.h \
    include/Value.h \
    include/Bit.h \
    include/AVRStudioXMLParser.h \
    include/AVRFactory.h \
    include/ProgrammingInterface.h \
    include/splash.h

FORMS    += \
    forms/maindialog.ui \
    forms/simppgmdlg.ui \
    forms/AVRDudeErrorWindow.ui

INCLUDEPATH = include

include(qextserialport-1.2beta1/src/qextserialport.pri)

OBJECTS_DIR = output/obj
MOC_DIR = output/moc
RCC_DIR = output/resources
DESTDIR = output

RESOURCES += \
    Resources.qrc

QMAKE_CXXFLAGS += -Wall \
    -Werror \
    -Os
