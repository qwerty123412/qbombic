#include "qgameserver.h"

#include "qplayerinfo.h"

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

        comm->registerRequest(REQ_LOGIN, std::bind(&QGameServer::onLogin, this, _1));
        comm->registerRequest(REQ_GET_PLAYERS, std::bind(&QGameServer::onGetPlayers, this, _1));
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
        delete(player);
    }

    communications.remove(&comm);

    garbageComms.insert(&comm);
}

void QGameServer::onLogin(std::shared_ptr<QJsonRequest> request)
{
    if (players.contains(request->getComm())) //already registred
    {
        request->sendResponse(RESP_FAILED);
        return;
    }

    QString name = request->getData()->toMap()["name"].toString();
    QPlayer* player = new QPlayer(this, request->getComm(), name);
    players.insert(request->getComm(), player);

    request->sendResponse(RESP_OK);
}

void QGameServer::onGetPlayers(std::shared_ptr<QJsonRequest> request)
{
    QVariantList plays;

    QPlayerInfo info;
    for(auto client : players.values())
    {
        client->writeInfo(info);

        plays << QJson::QObjectHelper::qobject2qvariant(&info);
    }

    request->sendResponse(RESP_OK, plays);
}

void QGameServer::cleanGarbageComms()
{
    for(auto c : garbageComms)
        delete(c);
    garbageComms.clear();
}
