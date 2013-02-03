#include "qplayerinfo.h"

QPlayerInfo::QPlayerInfo(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<QPlayerInfo>("QPlayerInfo");
}

QPlayerInfo::QPlayerInfo(const QPlayerInfo& another) :
    QObject(another.parent()), name(another.name)
{
}

QPlayerInfo::~QPlayerInfo()
{

}

