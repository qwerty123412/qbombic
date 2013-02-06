#ifndef QGAMESERVER_H
#define QGAMESERVER_H

#include <QObject>
#include <QVariant>
#include <QSet>
#include <QTcpServer>
#include <QTcpSocket>
#include <memory>
#include <functional>
#include <iostream>
#include <qjson/qobjecthelper.h>

#include "qplayer.h"
#include "qgame.h"
#include "qjsoncommunication.h"

class QGameServer : public QObject
{
    Q_OBJECT
public:
    explicit QGameServer(QObject *parent = 0);
    virtual ~QGameServer();

    void broadcastNotification(const QString& notif, const QVariant& data = QVariant());

    void onLogin(std::shared_ptr<QJsonRequest> request);
    QList<QPlayer*> getPlayers() const { return players.values(); }
    QList<QGame*> getGames() const { return games.values(); }

    QGame* createGame(QPlayer* player, const QString& name);
    QGame* getGame(const QString& name);
    void closeGame(QPlayer* player);

    QVariantList getGameList() const;
    void gameListChanged();

signals:
public slots:

    void onNewConnect();
    void onDisconnected(QJsonCommunication& comm);

private:
    void cleanGarbageComms();


    QSet<QJsonCommunication*> communications;
    QMap<QJsonCommunication*, QPlayer*> players;
    QMap<QString, QGame*> games;
    QTcpServer tcp_server;
    QSet<QJsonCommunication*> garbageComms;

};

#endif // QGAMESERVER_H
