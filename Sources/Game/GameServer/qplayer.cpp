#include "qplayer.h"

#include "qgameserver.h"

#include <RequestsResponses.h>

using namespace std::placeholders;

QPlayer::QPlayer(QGameServer* server, QJsonCommunication *comm, const QString name, QObject *parent):
    QObject(parent), server(server), comm(comm), name(name), game(nullptr)
{
    comm->registerNotification(Notifications::MESSAGE, std::bind(&QPlayer::onMessage, this, _1));
    comm->registerNotification(Notifications::QUIT_GAME, std::bind(&QPlayer::onGameLeave, this, _1));

    comm->registerRequest(Request::GET_PLAYERS, std::bind(&QPlayer::onGetPlayerList, this, _1));
    comm->registerRequest(Request::CREATE_GAME, std::bind(&QPlayer::onGameCreate, this, _1));
    comm->registerRequest(Request::JOIN_GAME, std::bind(&QPlayer::onGameJoin, this, _1));
    comm->registerRequest(Request::START_GAME, std::bind(&QPlayer::onGameStart, this, _1));
}

QPlayer::~QPlayer()
{
    if (game)
    {
        server->closeGame(this);
    }
}

void QPlayer::onGetPlayerList(std::shared_ptr<QJsonRequest> request)
{
    QVariantList plays;

    QPlayerInfo info;
    for(const QPlayer* client : server->getPlayers())
    {
        client->writeInfo(info);

        plays << QJson::QObjectHelper::qobject2qvariant(&info);
    }

    request->sendResponse(Response::OK, plays);
}

void QPlayer::onGameStart(std::shared_ptr<QJsonRequest> req)
{
    if (!game)
    {
        req->sendResponse(Response::FAILED);
        return;
    }

    if (!game->start())
    {
        req->sendResponse(Response::FAILED);
        return;
    }


    req->sendResponse(Response::OK);
}

void QPlayer::onMessage(const QVariant &messageData)
{
    QVariantMap map = messageData.toMap();
    map.insert("from", name);
    server->broadcastNotification(Notifications::MESSAGE, map);
}

void QPlayer::writeInfo(QPlayerInfo &info) const
{
    info.setName(name);
}

void QPlayer::onGameCreate(std::shared_ptr<QJsonRequest> req)
{
    if (game)
    {
        req->sendResponse(Response::FAILED);
        return;
    }

    QGame* g = server->createGame(this, extractQVariantItem(req, "name"));
    if (!g)
    {
        req->sendResponse(Response::FAILED);
        return;
    }

    g->join(this);

    game = g;
    req->sendResponse(Response::OK);
}

void QPlayer::onGameJoin(std::shared_ptr<QJsonRequest> req)
{
    QGame* g = server->getGame(extractQVariantItem(req, "name"));
    if (!g || !g->join(this))
    {
        req->sendResponse(Response::FAILED);
        return;
    }
    game = g;
    req->sendResponse(Response::OK);
    emit server->gameListChanged();
}

void QPlayer::onGameLeave(const QVariant &)
{
    if (game->getCreator() == this)
        server->closeGame(this);
    else
        game->leave(this);
    game = nullptr;
    emit server->gameListChanged();
}

void QPlayer::onGameCommand(const QVariant &command)
{
    game->command(this, extractQVariantItem(command, "command"));
}

/*
void onGameCommand(const QVariant& command);//just notification
*/






