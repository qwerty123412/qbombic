#include "qplayer.h"

#include "qgameserver.h"

#include <RequestsResponses.h>

using namespace std::placeholders;

QPlayer::QPlayer(QGameServer* server, QJsonCommunication *comm, const QString name, QObject *parent):
    QObject(parent), server(server), comm(comm), name(name), game(nullptr)
{
    comm->registerNotification(Notifications::MESSAGE, std::bind(&QPlayer::onMessage, this, _1));
    comm->registerNotification(Notifications::QUIT_GAME, std::bind(&QPlayer::onGameLeave, this, _1));
    comm->registerNotification(Notifications::GAME_STATE, std::bind(&QPlayer::onGameState, this, _1));
    comm->registerNotification(Notifications::GAME_STOPPED, std::bind(&QPlayer::onGameStop, this, _1));

    comm->registerRequest(Request::GET_PLAYERS, std::bind(&QPlayer::onGetPlayerList, this, _1));
    comm->registerRequest(Request::CREATE_GAME, std::bind(&QPlayer::onGameCreate, this, _1));
    comm->registerRequest(Request::JOIN_GAME, std::bind(&QPlayer::onGameJoin, this, _1));
    comm->registerRequest(Request::START_GAME, std::bind(&QPlayer::onGameStart, this, _1));
    comm->registerRequest(Request::GET_GAMES, std::bind(&QPlayer::onGetGameList, this, _1));

    //  game notifications
    comm->registerNotification(GameEvents::MOVE_DOWN, std::bind(&QPlayer::onMove<QGame::Directions>, this, _1, QGame::Directions::DOWN));
    comm->registerNotification(GameEvents::MOVE_LEFT, std::bind(&QPlayer::onMove<QGame::Directions>, this, _1, QGame::Directions::LEFT));
    comm->registerNotification(GameEvents::MOVE_RIGHT, std::bind(&QPlayer::onMove<QGame::Directions>, this, _1, QGame::Directions::RIGHT));
    comm->registerNotification(GameEvents::MOVE_UP, std::bind(&QPlayer::onMove<QGame::Directions>, this, _1, QGame::Directions::UP));
    comm->registerNotification(GameEvents::BOMB_LAID, std::bind(&QPlayer::onPushBomb, this, _1));

}

QPlayer::~QPlayer()
{
    if (game)
        onGameLeave(QVariant());
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

    game->start();

    req->sendResponse(Response::OK);
}

void QPlayer::onGameStop(const QVariant &)
{
    if (game)
    {
        game->stop();
    }
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
    req->sendResponse(Response::OK, QVariant(game->getStarted()));
    server->gameListChanged();
}

void QPlayer::onGameLeave(const QVariant &)
{
    if (!game)
        return;
    if (game->getCreator() == this)
        server->closeGame(this);
    else
        game->leave(this);
    game = nullptr;
    server->gameListChanged();
}

void QPlayer::onGameState(const QVariant &state)
{
    if (game)
        game->broadcastNotification(nullptr, Notifications::GAME_STATE, state);
}

template<typename Direction>
void QPlayer::onMove(const QVariant&, Direction direction)
{
    if (game)
        game->move(this, direction);
}

void QPlayer::onPushBomb(const QVariant &)
{
    if (game)
        game->bomb(this);
}

void QPlayer::onGetGameList(std::shared_ptr<QJsonRequest> req)
{
    req->sendResponse(Response::OK, server->getGameList());
}




