#include "qgameinfo.h"

QGameInfo::QGameInfo(QObject* parent) :
    QObject(parent)
{
}

QGameInfo::QGameInfo(const QGameInfo &other)
    : name(other.name), creator(other.creator), players(other.players)
{
}

QGameInfo::~QGameInfo()
{
}
