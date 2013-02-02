#ifndef QPLAYER_H
#define QPLAYER_H

#include <QObject>
#include <QVariant>
#include <functional>
#include <memory>

#include <qjsoncommunication.h>
#include <qplayerinfo.h>

class QGameServer;
class QGame;

class QPlayer : public QObject
{
    Q_OBJECT
public:
    explicit QPlayer(QGameServer* server, QJsonCommunication* comm, const QString name, QObject *parent = 0);
    ~QPlayer();

    const QString& getName() { return name; }
    QJsonCommunication* getComm() { return comm; }
    QGame* getGame() { return game; }

    void onMessage(const QVariant& messageData);

    void writeInfo(QPlayerInfo& info) const;

    void onGetPlayerList(std::shared_ptr<QJsonRequest> req);

    void onGameStart(std::shared_ptr<QJsonRequest> req);
    void onGameCreate(std::shared_ptr<QJsonRequest> req);
    void onGameJoin(std::shared_ptr<QJsonRequest> req);
    void onGameLeave(const QVariant&);// just notification
    void onGameCommand(const QVariant& command);//just notification


    void onGameState(const QVariant& state);

    void gameQuit() { game = nullptr; }

signals:
    
public slots:


private:
    QGameServer* server;
    QJsonCommunication* comm;
    QString name;
    QGame* game;
};

#endif // QPLAYER_H
