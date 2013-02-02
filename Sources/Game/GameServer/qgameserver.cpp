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
    if (player != nullptr)
    {
        players.remove(&comm);
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
    QPlayer* player = new QPlayer(this, comm, name);

    request->sendResponse(Response::OK);
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
    games.remove(player->getGame()->getName());
    delete player->getGame();
    gameListChanged();
}

void QGameServer::gameListChanged()
{
    QGameInfo info;
    QVariantList infos;
    for(QGame* game : games.values())
    {
        game->writeInfo(info);
        infos << QJson::QObjectHelper::qobject2qvariant(&info);
    }
    broadcastNotification(Notifications::GAME_LIST, infos);
}

QGame* QGameServer::getGame(const QString &name)
{
    return games.contains(name) ? games[name] : nullptr;
}








