#-------------------------------------------------
#
# Project created by QtCreator 2012-12-02T17:37:24
#
#-------------------------------------------------

QT       += core gui network

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -Wunused-parameter

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TestServer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ProtocolLib/release/ -lProtocolLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ProtocolLib/debug/ -lProtocolLib
else:symbian: LIBS += -lProtocolLib
else:unix: LIBS += -L$$OUT_PWD/../ProtocolLib/ -lProtocolLib

INCLUDEPATH += $$PWD/../ProtocolLib
DEPENDPATH += $$PWD/../ProtocolLib
