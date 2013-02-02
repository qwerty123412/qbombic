#-------------------------------------------------
#
# Project created by QtCreator 2012-12-03T18:56:28
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=c++11

TARGET = GameClient
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    logindialog.cpp

HEADERS  += mainwindow.h \
    logindialog.h

FORMS    += mainwindow.ui \
    logindialog.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../GameShared/release/ -lGameShared
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../GameShared/debug/ -lGameShared
else:symbian: LIBS += -lGameShared
else:unix: LIBS += -L$$OUT_PWD/../GameShared/ -lGameShared

INCLUDEPATH += $$PWD/../GameShared
DEPENDPATH += $$PWD/../GameShared

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ProtocolLib/release/ -lProtocolLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ProtocolLib/debug/ -lProtocolLib
else:symbian: LIBS += -lProtocolLib
else:unix: LIBS += -L$$OUT_PWD/../ProtocolLib/ -lProtocolLib

INCLUDEPATH += $$PWD/../ProtocolLib
DEPENDPATH += $$PWD/../ProtocolLib



win32:CONFIG(release, debug|release): LIBS += -L/usr/lib/x86_64-linux-gnu/release/ -lqjson
else:win32:CONFIG(debug, debug|release): LIBS += -L/usr/lib/x86_64-linux-gnu/debug/ -lqjson
else:symbian: LIBS += -lqjson
else:unix: LIBS += -L/usr/lib/x86_64-linux-gnu/ -lqjson

INCLUDEPATH += /usr/include
DEPENDPATH += /usr/include

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../GameUI/release/ -lGameUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../GameUI/debug/ -lGameUI
else:symbian: LIBS += -lGameUI
else:unix: LIBS += -L$$OUT_PWD/../../GameUI/ -lGameUI

INCLUDEPATH += $$PWD/../../GameUI
DEPENDPATH += $$PWD/../../GameUI
