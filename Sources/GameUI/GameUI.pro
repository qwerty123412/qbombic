TEMPLATE = lib
INCLUDEPATH += .

TARGET = GameUI

DEFINES += GAMEUI_LIBRARY

# Input
HEADERS += ledmeter.h sprites.h toplevel.h view.h animateditem.h \
    bomb.h \
    explosion.h \
    character.h \
    gameuiconstants.h \
    powerup.h \
    block.h
SOURCES += ledmeter.cpp main.cpp toplevel.cpp view.cpp animateditem.cpp \
    bomb.cpp \
    explosion.cpp \
    character.cpp \
    gameuiconstants.cpp \
    powerup.cpp \
    block.cpp

QMAKE_CXXFLAGS += -std=c++0x

RESOURCES += portedasteroids.qrc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/graphicsview/portedasteroids
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS portedasteroids.pro bg.png sounds sprites
sources.path = $$[QT_INSTALL_EXAMPLES]/graphicsview/portedasteroids

INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)

simulator: warning(This example might not fully work on Simulator platform)
