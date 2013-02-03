#include "qgameinfo.h"
#include <qjson/qobjecthelper.h>

QGameInfo::QGameInfo(QObject* parent) :
    QObject(parent)
{
}

QGameInfo::QGameInfo(const QGameInfo &another) :
    QObject(another.parent()), name(another.name)
{
}

QGameInfo::~QGameInfo()
{
}
