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
#include "qjsoncommunication.h"

class QGameServer : public QObject
{
    Q_OBJECT
public:
    explicit QGameServer(QObject *parent = 0);
    virtual ~QGameServer();

    void broadcastNotification(const QString& notif, const QVariant& data);

    void onLogin(std::shared_ptr<QJsonRequest> request);
    void onGetPlayers(std::shared_ptr<QJsonRequest> request);

signals:
    
public slots:

    void onNewConnect();
    void onDisconnected(QJsonCommunication& comm);

private:
    void cleanGarbageComms();

    QSet<QJsonCommunication*> communications;
    QMap<QJsonCommunication*, QPlayer*> players;
    QTcpServer tcp_server;
    QSet<QJsonCommunication*> garbageComms;

};

#endif // QGAMESERVER_H
