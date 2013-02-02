#include "qplayerinfo.h"

QPlayerInfo::QPlayerInfo(QObject *parent) :
    QObject(parent)
{
}

QPlayerInfo::QPlayerInfo(const QPlayerInfo& another) :
    QObject(another.parent()), name(another.name)
{
}
