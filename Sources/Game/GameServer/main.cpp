#include <QCoreApplication>
#include <QDebug>
#include <iostream>

#include "qgameserver.h"
#include "gameshared.h"

int main(int argc, char *argv[])
{
    GameShared::Init();
    QCoreApplication a(argc, argv);

    QGameServer server;

    return a.exec();
}
