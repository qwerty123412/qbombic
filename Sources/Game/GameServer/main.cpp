#include <QCoreApplication>
#include <QDebug>
#include <iostream>

#include "qgameserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QGameServer server;

    return a.exec();
}
