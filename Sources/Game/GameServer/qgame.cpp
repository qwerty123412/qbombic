#include "qgame.h"

#include "qgameserver.h"
#include <qjson/qobjecthelper.h>
#include "qgameinfo.h"

#include "RequestsResponses.h"

QGame::QGame(QGameServer *server, const QString &name, QPlayer *creator, QObject *parent) :
    QObject(parent), server(server), comm(creator->getComm()), name(name), creator(creator), members(), maximum(6)
{
    members.insert(creator);
}
QGame::~QGame()
{
    broadcastNotification(creator, Notifications::GAME_CLOSED);
    for(QPlayer* player : members)
    {
        player->gameQuit();
    }
}

void QGame::writeInfo(QGameInfo &info) const
{
    info.setPlayers(members.count());
    info.setCreator(creator->getName());

    info.setName(name);
}

void QGame::broadcastNotification(const QPlayer *except, const QString &notif, const QVariant &data)
{
    for(QPlayer* player : members)
    {
        if (player == except)
            continue;

        player->getComm()->sendNotification(notif, data);
    }
}

bool QGame::join(QPlayer *player)
{
    if (members.size() >= maximum)
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

bool QGame::start()
{
    broadcastNotification(nullptr, Notifications::GAME_STARTED);
    return true;
}

