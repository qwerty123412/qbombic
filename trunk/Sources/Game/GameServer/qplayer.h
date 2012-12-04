#ifndef QPLAYER_H
#define QPLAYER_H

#include <QObject>
#include <QVariant>
#include <functional>
#include <memory>

#include <qjsoncommunication.h>
#include <qplayerinfo.h>

class QGameServer;

class QPlayer : public QObject
{
    Q_OBJECT
public:
    explicit QPlayer(QGameServer* server, QJsonCommunication* comm, const QString name, QObject *parent = 0);
    ~QPlayer();

    const QString& getName() { return name; }
    QJsonCommunication* getComm() { return comm; }

    void onMessage(const QVariant& messageData);

    void writeInfo(QPlayerInfo& info) const;

signals:
    
public slots:


private:
    QGameServer* server;
    QJsonCommunication* comm;
    QString name;
};

#endif // QPLAYER_H
