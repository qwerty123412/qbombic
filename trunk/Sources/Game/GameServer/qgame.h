#ifndef QGAME_H
#define QGAME_H



#include <QObject>
#include <QVariant>
#include <functional>
#include <memory>
#include <QSet>

#include <qjsoncommunication.h>
#include <qplayerinfo.h>
#include <qgameinfo.h>
#include <qplayer.h>

class QGameServer;

class QGame : public QObject
{
    Q_OBJECT
public:
    explicit QGame(QGameServer* server, const QString& name, QPlayer* creator, QObject *parent = 0);
    ~QGame();

    const QString& getName() const { return name; }
    QJsonCommunication* getComm() { return comm; }
    QPlayer* getCreator() { return creator; }

    bool join(QPlayer* player);
    void leave(QPlayer* player);
    void command(QPlayer* player, const QString& command);
    bool start();

    void writeInfo(QGameInfo& info) const;

    void broadcastNotification(const QPlayer* except, const QString& notif, const QVariant& data = QVariant());

signals:

public slots:


private:
    QGameServer* server;
    QJsonCommunication* comm;
    QString name;
    QPlayer* creator;
    QSet<QPlayer*> members;
    const int maximum;
};

#endif // QGAME_H
