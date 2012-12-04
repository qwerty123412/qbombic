#-------------------------------------------------
#
# Project created by QtCreator 2012-12-03T18:54:16
#
#-------------------------------------------------

QT       += core network

QT       -= gui

QMAKE_CXXFLAGS += -std=c++11

TARGET = GameServer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    qplayer.cpp \
    qgameserver.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ProtocolLib/release/ -lProtocolLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ProtocolLib/debug/ -lProtocolLib
else:symbian: LIBS += -lProtocolLib
else:unix: LIBS += -L$$OUT_PWD/../ProtocolLib/ -lProtocolLib

INCLUDEPATH += $$PWD/../ProtocolLib
DEPENDPATH += $$PWD/../ProtocolLib

HEADERS += \
    qplayer.h \
    qgameserver.h

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../GameShared/release/ -lGameShared
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../GameShared/debug/ -lGameShared
else:symbian: LIBS += -lGameShared
else:unix: LIBS += -L$$OUT_PWD/../GameShared/ -lGameShared

INCLUDEPATH += $$PWD/../GameShared
DEPENDPATH += $$PWD/../GameShared


win32:CONFIG(release, debug|release): LIBS += -L/usr/lib/x86_64-linux-gnu/release/ -lqjson
else:win32:CONFIG(debug, debug|release): LIBS += -L/usr/lib/x86_64-linux-gnu/debug/ -lqjson
else:symbian: LIBS += -lqjson
else:unix: LIBS += -L/usr/lib/x86_64-linux-gnu/ -lqjson

INCLUDEPATH += /usr/include
DEPENDPATH += /usr/include
