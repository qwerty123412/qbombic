#include "qgame.h"

#include "qgameserver.h"
#include <qjson/qobjecthelper.h>
#include "qgameinfo.h"

#include "RequestsResponses.h"

QGame::QGame(QGameServer *server, const QString &name, QPlayer *creator, QObject *parent) :
    QObject(parent), server(server), comm(creator->getComm()), name(name), creator(creator), members(), maximum(6)
{
}
QGame::~QGame()
{
    for(QPlayer* player : members)
    {
        player->getComm()->sendNotification(Notifications::GAME_CLOSED);
        player->gameQuit();
    }
}

void QGame::writeInfo(QGameInfo &info) const
{
    QPlayerInfo playerInfo;
    QList<QPlayerInfo> _members;
    for( QPlayer* player : members )
    {
        player->writeInfo(playerInfo);
        _members.push_back(playerInfo);
    }
    info.setMembers(_members);

    creator->writeInfo(playerInfo);
    info.setCreator(playerInfo);

    info.setName(name);
}

bool QGame::join(QPlayer *player)
{
    if (members.size() == 6)
        return false;
    members.insert(player);
    server->gameListChanged();
    return true;
}

void QGame::leave(QPlayer *player)
{
    members.remove(player);
    server->gameListChanged();
}

void QGame::command(QPlayer *player, const QString &command)
{
    throw int(0);// not implemented yet
}
