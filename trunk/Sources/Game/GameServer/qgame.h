#ifndef QGAME_H
#define QGAME_H



#include <QObject>
#include <QVariant>
#include <QVector>
#include <functional>
#include <memory>
#include <QSet>
#include <QTimer>

#include <qjsoncommunication.h>
#include <qplayerinfo.h>
#include <qgameinfo.h>
#include <qplayer.h>
#include <qgameobject.h>
#include <qcoordinations.h>

class QGameServer;

class QGame : public QObject
{
    Q_OBJECT
public:

    enum class Directions
    {
        DOWN,
        LEFT,
        RIGHT,
        UP
    };


    explicit QGame(QGameServer* server, const QString& name, QPlayer* creator, QObject *parent = 0);
    ~QGame();

    const QString& getName() const { return name; }
    QJsonCommunication* getComm() { return comm; }
    QPlayer* getCreator() { return creator; }

    bool join(QPlayer* player);
    void leave(QPlayer* player);
    void start();
    void stop();

    void writeInfo(QGameInfo& info) const;

    void broadcastNotification(const QPlayer* except, const QString& notif, const QVariant& data = QVariant());

    void move(QPlayer* player, Directions direction);
    void bomb(QPlayer* player);

    void sendPlayersInfo(QVariantMap& data);
    void sendBombsInfo(QVariantMap& data);
    void sendFiresInfo(QVariantMap& data);
    void sendPowerups(QVariantMap& data);
    void sendWalls(QVariantMap& data);

signals:

public slots:

    void refreshGameWorld();
    void sendWorldInfo();

private:

    void insertPlayerIntoGame(QPlayer* player);

    enum class PlayerEvents
    {
        MOVE_DOWN,
        MOVE_LEFT,
        MOVE_RIGHT,
        MOVE_UP,
        PUSH_BOMB,
        NOOP
    };

    bool started;
    QGameServer* server;
    QJsonCommunication* comm;
    QString name;
    QPlayer* creator;
    QSet<QPlayer*> members;
    const int maximum;
    QMap<QPlayer*, PlayerEvents> lastEvent;
    unsigned ticks;


    QMap<QCoordinations, QGameObject> area;
    QMap<QCoordinations, QGameCharacter*> players;
    QMap<QPlayer*, QGameCharacter*> characters;// ownership of QGameCharacter


    QTimer timer;
};

#endif // QGAME_H
