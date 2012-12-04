#-------------------------------------------------
#
# Project created by QtCreator 2012-12-02T17:36:51
#
#-------------------------------------------------

QT       += network

QT       -= gui

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -Wunused-parameter

TARGET = ProtocolLib
TEMPLATE = lib

DEFINES += PROTOCOLLIB_LIBRARY

SOURCES += protocollib.cpp \
    qjsonsocket.cpp \
    qjsoncommunication.cpp \
    qjsonrequest.cpp \
    qjsonrequestobject.cpp

HEADERS += protocollib.h\
        ProtocolLib_global.h \
    qjsonsocket.h \
    qjsoncommunication.h \
    qjsonrequest.h \
    qjsonrequestobject.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE4C32EE7
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = ProtocolLib.dll
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

win32:CONFIG(release, debug|release): LIBS += -L/usr/lib/x86_64-linux-gnu/release/ -lqjson
else:win32:CONFIG(debug, debug|release): LIBS += -L/usr/lib/x86_64-linux-gnu/debug/ -lqjson
else:symbian: LIBS += -lqjson
else:unix: LIBS += -L/usr/lib/x86_64-linux-gnu/ -lqjson

INCLUDEPATH += /usr/include
DEPENDPATH += /usr/include
