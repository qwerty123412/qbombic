#ifndef REQUESTSRESPONSES_H
#define REQUESTSRESPONSES_H

#include <QString>
#include <QVariant>
#include <memory>
#include <qjsonrequest.h>


inline static QString extractQVariantItem(const QVariant& var, const QString& key)
{
    return var.toMap()[key].toString();
}

inline static QString extractQVariantItem(const std::shared_ptr<QJsonRequest> req, const QString& key)
{
    return extractQVariantItem(*req->getData(), key);
}

struct Response
{
    static const QString OK;
    static const QString FAILED;
};

struct Request
{
    static const QString LOGIN;//send client
    static const QString GET_PLAYERS;//send client
    static const QString GET_GAMES;//send client
    static const QString CREATE_GAME;//send client
    static const QString JOIN_GAME;//send client
    static const QString START_GAME;//send client
};

struct Notifications
{
    static const QString NEW_PLAYER;// server => clients - name of coming client
    static const QString QUIT_PLAYER;// server => clients - name of leaving client
    static const QString GAME_CLOSED;// server => clients - game room destroyed
    static const QString GAME_STARTED;// server => clients - game started
    static const QString GAME_STOPPED;// server => clients - game stopped, game room still lives
    static const QString GAME_LIST;// server => clients - list of available game rooms
    static const QString MESSAGE;// client -> server => clients
    static const QString QUIT_GAME;// client -> server - I am leaving this game
    static const QString GAME_STATE; // server => clients - actual game state
};

struct GameEvents// client -> server
{
    static const QString MOVE_UP;
    static const QString MOVE_DOWN;
    static const QString MOVE_LEFT;
    static const QString MOVE_RIGHT;
    static const QString PUSH_BOMB;
};

struct GameDataObjects// server -> client
{
    static const QString BOMB;
    static const QString FIRE;
    static const QString PLAYER;
    static const QString WALL;
    static const QString POWERUP;

    static const QString P_NAME;
    static const QString P_COORDINATIONS;
    static const QString P_BOMBS;
    static const QString P_DEATHS;
    static const QString P_KILLS;
};

#endif // REQUESTSRESPONSES_H
