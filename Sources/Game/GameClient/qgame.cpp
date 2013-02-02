#include "qgame.h"

QGame::QGame(QObject *parent) :
    QObject(parent)
{
}

void QGame::start()
{
    window.show();
}
