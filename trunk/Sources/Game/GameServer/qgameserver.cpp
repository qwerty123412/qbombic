#include "qgameserver.h"

#include "qplayerinfo.h"
#include "qgameinfo.h"
#include "RequestsResponses.h"

using namespace std::placeholders;


QGameServer::QGameServer(QObject *parent) :
    QObject(parent), tcp_server(parent)
{
    qDebug() << "Starting...";

    connect(&tcp_server, SIGNAL(newConnection()), SLOT(onNewConnect()));

    if (!tcp_server.listen(QHostAddress::Any, 9876)) //todo: settings
    {
        qDebug() << "Cannot listen";
        exit(1);
    }

    qDebug() << "Listening...";
}

QGameServer::~QGameServer()
{
    cleanGarbageComms();

    for(auto client : communications)
        delete(client);
}

void QGameServer::broadcastNotification(const QString &notif, const QVariant& data)
{
    for(auto client : players.values())
    {
        client->getComm()->sendNotification(notif, data);
    }
}

void QGameServer::onNewConnect()
{
    while(tcp_server.hasPendingConnections())
    {
        QTcpSocket* socket = tcp_server.nextPendingConnection();

        qDebug() << "Connected... " << socket->peerAddress();

        QJsonCommunication* comm = new QJsonCommunication(socket);
        communications.insert(comm);

        connect(comm, SIGNAL(disconnected(QJsonCommunication&)), SLOT(onDisconnected(QJsonCommunication&)));

        comm->registerRequest(Request::LOGIN, std::bind(&QGameServer::onLogin, this, _1));
    }

    cleanGarbageComms();
}

void QGameServer::onDisconnected(QJsonCommunication& comm)
{
    qDebug() << "Disconnected... ";

    QPlayer* player = players[&comm];
    players.remove(&comm);
    if (player != nullptr)
    {
        QPlayerInfo info;
        player->writeInfo(info);
        broadcastNotification(Notifications::QUIT_PLAYER, QJson::QObjectHelper::qobject2qvariant(&info));
        delete(player);
    }

    communications.remove(&comm);

    garbageComms.insert(&comm);
}

void QGameServer::onLogin(std::shared_ptr<QJsonRequest> request)
{
    QJsonCommunication* comm = request->getComm();
    if (players.contains(comm)) //already registred
    {
        request->sendResponse(Response::FAILED);
        return;
    }
    QString name = extractQVariantItem(request, "name");
    for (QPlayer* player : players.values())// check for existing nicknames
    {
        if (player->getName() == name)
        {
            request->sendResponse(Response::FAILED);
            return;
        }
    }
    /*
    // Response have to be send before sending game list -
    // it is necessary to register notification on client side
    */
    request->sendResponse(Response::OK);
    QPlayer* player = new QPlayer(this, comm, name);

    QPlayerInfo info;
    player->writeInfo(info);
    broadcastNotification(Notifications::NEW_PLAYER, QJson::QObjectHelper::qobject2qvariant(&info));
    players.insert(request->getComm(), player);
}

void QGameServer::cleanGarbageComms()
{
    for(auto c : garbageComms)
        delete(c);
    garbageComms.clear();
}

QGame* QGameServer::createGame(QPlayer *player, const QString &name)
{
    if (games.contains(name))
        return nullptr;

    QGame* ret = new QGame(this, name, player);
    games.insert(name, ret);
    gameListChanged();
    return ret;
}

void QGameServer::closeGame(QPlayer *player)
{
    if (!player->getGame() || player->getGame()->getCreator() != player)// close only if player is also creatro
        return;
    games.remove(player->getGame()->getName());
    delete player->getGame();
    gameListChanged();
}

QVariantList QGameServer::getGameList() const
{
    QGameInfo info;
    QVariantList infos;
    for(QGame* game : games.values())
    {
        game->writeInfo(info);
        infos << QJson::QObjectHelper::qobject2qvariant(&info);
    }
    return infos;
}

void QGameServer::gameListChanged()
{
    broadcastNotification(Notifications::GAME_LIST, getGameList());
}

QGame* QGameServer::getGame(const QString &name)
{
    return games.contains(name) ? games[name] : nullptr;
}








