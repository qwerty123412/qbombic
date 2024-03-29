#-------------------------------------------------
#
# Project created by QtCreator 2012-12-03T18:55:01
#
#-------------------------------------------------

QT       += network

QT       -= gui

QMAKE_CXXFLAGS += -std=c++0x

TARGET = GameShared
TEMPLATE = lib

DEFINES += GAMESHARED_LIBRARY

SOURCES += gameshared.cpp \
    qplayerinfo.cpp \
    qgameinfo.cpp \
    RequestsResponses.cpp \
    qgamestate.cpp

HEADERS += gameshared.h\
        GameShared_global.h \
    qplayerinfo.h \
    RequestsResponses.h \
    qgameinfo.h \
    qgamestate.h \
    qcoordinations.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xECF4D042
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = GameShared.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

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
