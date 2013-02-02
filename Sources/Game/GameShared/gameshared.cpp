#include "gameshared.h"
#include "QMetaType"
#include "qplayerinfo.h"
#include "qgameinfo.h"

GameShared::GameShared()
{
}

void GameShared::Init()
{
    qRegisterMetaType<QPlayerInfo>("QPlayerInfo");
    qRegisterMetaType<QGameInfo>("QGameInfo");
}

