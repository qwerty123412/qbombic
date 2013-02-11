#include "qgame.h"

#include "qgameserver.h"
#include <qjson/qobjecthelper.h>
#include "qgameinfo.h"

#include "RequestsResponses.h"
#include "qcoordinations.h"
#include "qgamecharacter.h"

#include "qgamesettings.h" // compile settings of timing and game area

QGame::QGame(QGameServer *server, const QString &name, QPlayer *creator, QObject *parent) :
    QObject(parent), started(false), server(server), comm(creator->getComm()), name(name),
    creator(creator), members(), maximum(6), ticks(0), area(), players(), characters(), timer(parent)
{
    members.insert(creator);
    connect(&timer, SIGNAL(timeout()), SLOT(refreshGameWorld()));
}
QGame::~QGame()
{
    timer.stop();
    broadcastNotification(creator, Notifications::GAME_CLOSED);
    for(QPlayer* player : members)
    {
        player->gameQuit();
    }
    for (QGameCharacter* character : characters)
        delete character;
}

void QGame::writeInfo(QGameInfo &info) const
{
    info.setPlayers(members.count());
    info.setCreator(creator->getName());
    info.setStarted(started);
    info.setName(name);
}

void QGame::broadcastNotification(const QPlayer *except, const QString &notif, const QVariant &data)
{
    for(QPlayer* player : members)
    {
        if (player == except)
            continue;

        player->getComm()->sendNotification(notif, data);
    }
}

void QGame::move(QPlayer *player, QGame::Directions direction)
{
    PlayerEvents event = PlayerEvents::NOOP;
    switch(direction)
    {
    case Directions::DOWN:
        event = PlayerEvents::MOVE_DOWN;
        break;
    case Directions::LEFT:
        event = PlayerEvents::MOVE_LEFT;
        break;
    case Directions::RIGHT:
        event = PlayerEvents::MOVE_RIGHT;
        break;
    case Directions::UP:
        event = PlayerEvents::MOVE_UP;
        break;
    }
    lastEvent[player] = event;
}

void QGame::bomb(QPlayer *player)
{
    lastEvent[player] = PlayerEvents::PUSH_BOMB;
}

bool QGame::join(QPlayer *player)
{
    if (members.size() >= maximum || members.contains(player))
        return false;
    members.insert(player);

    if (started)
        insertPlayerIntoGame(player);

    server->gameListChanged();
    return true;
}

void QGame::leave(QPlayer *player)
{
    members.remove(player);
    if (started)
    {
        QGameCharacter* character = characters[player];
        players.remove(character->getCoordinations());
        lastEvent.remove(player);
        characters.remove(player);
    }
    server->gameListChanged();
}

void QGame::start()
{
    // generate random walls and random powerups
    unsigned x, y;
    unsigned count = 2;
    QCoordinations last;

    while (count)
    {
        index(qrand() % size(SIZE_X - 7, SIZE_Y - 7), x, y, SIZE_X - 7);

        if (count != 2 && !(
                    last.Y() > y + 7 || // is up to it
                    last.Y() + 7 < y || // is down to it
                    last.X() > x + 7 || // is left to it
                    last.X() + 7 < x    // is right to it
                    ))
            continue;

        for (int i = 0; i < 5; ++i)
        {
            QCoordinations pos(x + i, y, this);
            area.insert(pos, QGameObject(new QGameUndestroyable(), pos));
        }

        for (int i = 0; i < 2; ++i)
        {
            for (int j = 0; j < 5; j+= 2)
            {
                QCoordinations pos(x + j, y + (2 * (i + 1)), this);
                area.insert(pos, QGameObject(new QGameUndestroyable(), pos));
            }
        }
        last.X() = x;
        last.Y() = y;

        --count;
    }

    count = 5;
    while (count)
    {
        index(qrand() % size(), x, y);
        if (area.contains(QCoordinations(x, y)))
            continue;
        area.insert(QCoordinations(x, y, this), QGameObject(new QGamePowerUp(), QCoordinations(x, y, this)));
        --count;        
    }

    count = size() / 10;
    while (count)
    {
        index(qrand() % size(), x, y);
        if (area.contains(QCoordinations(x, y))) {
            //qDebug() << "collision";
            continue;
        }
        area.insert(QCoordinations(x, y, this), QGameObject(new QGameWall(), QCoordinations(x, y, this)));
        --count;
        qDebug() << "wall generated at: x=" << x << " y=" << y;
    }

    for (QPlayer* player : members)
        insertPlayerIntoGame(player);


    timer.start(100);
    broadcastNotification(nullptr, Notifications::GAME_STARTED);
}

void QGame::stop()
{
    for (QGameCharacter* character : characters)
        delete character;
    started = false;
    timer.stop();
    area.clear();
    characters.clear();
    lastEvent.clear();
    broadcastNotification(nullptr, Notifications::GAME_STOPPED);
}

void QGame::insertPlayerIntoGame(QPlayer *player)
{
    unsigned x, y;
    while (true)
    {
        index(qrand() % size(), x, y);
        if (!area.contains(QCoordinations(x, y)) && !players.contains(QCoordinations(x, y)))
            break;
    }
    QCoordinations c(x, y, this);
    QGameCharacter* ch = new QGameCharacter(player, c);
    players.insert(c, ch);
    characters.insert(player, ch);
}

void QGame::refreshGameWorld()
{
    const QVector<QPair<int,int>> ways{ {-1, 0}, {1, 0} ,{0, -1}, {0, 1} };
    QList<QGameCharacter*> dead;

    // process fires
    for (QGameObject& object : area.values())
    {
        if (object.type() != QGameObject::FIRE)
            continue;

        QGameFire* fire = object.data<QGameFire>();
        if (!fire->firing())
            area.remove(object.getCoords());
        else if (players.contains(object.getCoords()))
        {
            dead.push_back(players[object.getCoords()]);
            players.remove(object.getCoords());
            fire->getOwner()->kill();
        }

    }

    // process bombs
    bool processBombs;
    do
    {
        processBombs = false;
        for(QGameObject& object : area.values())
        {
            if (object.type() != QGameObject::BOMB)
                continue;

            QGameBomb* bomb = object.data<QGameBomb>();
            if (!bomb->explosion())
                continue;

            processBombs = true;

            bomb->getOwner()->addBomb();
            QCoordinations center = object.getCoords();
            area.insert(center, QGameObject(new QGameFire(bomb->getOwner(), time2frame<QGameFire>()), center));

            for (QPair<int,int> way : ways)
            {
                QCoordinations actual(center);
                bool stop = false;
                for (int i = 0; i < 4; ++i)
                {
                    actual.X() += way.first;
                    actual.Y() += way.second;

                    if (!area.contains(actual) || area[actual].type() == QGameObject::POWERUP)
                    {
                        area.insert(actual, QGameObject(new QGameFire(bomb->getOwner(), time2frame<QGameFire>()), actual));
                        continue;
                    }
                    QGameObject& current = area[actual];

                    if (players.contains(actual))
                    {
                        dead.push_back(players[actual]);
                        players.remove(actual);
                        bomb->getOwner()->kill();
                        area.insert(actual, QGameObject(new QGameFire(bomb->getOwner(), time2frame<QGameFire>()), actual));
                    }
                    switch(current.type())
                    {
                    case QGameObject::BOMB:
                        current.data<QGameBomb>()->fired();
                    case QGameObject::FIRE:
                    case QGameObject::UNDESTROYABLE:
                        stop = true;
                        break;
                    case QGameObject::WALL:
                        area.insert(actual, QGameObject(new QGameFire(bomb->getOwner(), time2frame<QGameFire>()), actual));
                        stop = true;
                        break;
                    }
                    if (stop)
                        break;
                }
            }
        }
    } while (processBombs);

    //process deads
    for (QGameCharacter* character : dead)
    {
        character->die();
        unsigned x, y;
        while (true)
        {
            index(qrand() % size(), x, y);
            if (!area.contains(QCoordinations(x, y)) && !players.contains(QCoordinations(x, y)))
                break;
        }
        character->getCoordinations().X() = x;
        character->getCoordinations().Y() = y;
        players.insert(character->getCoordinations(), character);
    }
    dead.clear();

    // process events
    for (QGameCharacter* character : players.values())
    {
        if (!lastEvent.contains(character->getPlayer()))
            continue;

        if (lastEvent[character->getPlayer()] == PlayerEvents::NOOP)
            continue;
        if (lastEvent[character->getPlayer()] == PlayerEvents::PUSH_BOMB)
        {
            area.insert(character->getCoordinations(), QGameObject(new QGameBomb(character, time2frame<QGameBomb>()), character->getCoordinations()));
            continue;
        }
        QCoordinations newCoords(character->getCoordinations());
        switch(lastEvent[character->getPlayer()])
        {
        case PlayerEvents::MOVE_DOWN:
            newCoords.Y()++;
            break;
        case PlayerEvents::MOVE_LEFT:
            newCoords.X()--;
            break;
        case PlayerEvents::MOVE_RIGHT:
            newCoords.X()++;
            break;
        case PlayerEvents::MOVE_UP:
            newCoords.Y()--;
            break;
        default://avoid warning
            break;
        }
        if (newCoords.X() >= SIZE_X || newCoords.Y() >= SIZE_Y)// avoid leaving game area
            continue;

        if (players.contains(newCoords))
            continue;

        if (area.contains(newCoords) && (
                    area[newCoords].type() == QGameObject::WALL ||
                    area[newCoords].type() == QGameObject::BOMB ||
                    area[newCoords].type() == QGameObject::UNDESTROYABLE
                    )) {
            continue;
        }

        players.remove(character->getCoordinations());
        character->getCoordinations() = newCoords;
        players.insert(newCoords, character);
    }
    lastEvent.clear();

    // process powerups
    for (QGameObject& object : area.values())
    {
        if (object.type() != QGameObject::POWERUP)
            continue;

        if (!players.contains(object.getCoords()))
            continue;

        players[object.getCoords()]->addBomb();
        area.remove(object.getCoords());
    }
    if (ticks >= 100)
    {
        unsigned x, y;
        while (true)
        {
            index(qrand() % size(), x, y);
            if (!area.contains(QCoordinations(x, y)) && !players.contains(QCoordinations(x, y)))
                break;
        }
        area.insert(QCoordinations(x, y, this), QGameObject(new QGamePowerUp(), QCoordinations(x, y, this)));
        ticks = 0;
    }
    else
        ++ticks;


    sendWorldInfo();
}

void QGame::sendWorldInfo()
{
    QVariantMap data;

    QList<QGameObject> fires, bombs, powerups, walls, undestrs;
    for (QGameObject& object : area.values())
    {
        switch(object.type())
        {
        case QGameObject::BOMB:
            bombs << object;
            break;
        case QGameObject::FIRE:
            fires << object;
            break;
        case QGameObject::POWERUP:
            powerups << object;
            break;
        case QGameObject::WALL:
            walls << object;
            break;
        case QGameObject::UNDESTROYABLE:
            undestrs << object;
            break;
        }
    }
    sendPlayersInfo(data);
    sendFiresInfo(fires, data);
    sendBombsInfo(bombs, data);
    sendPowerups(powerups, data);
    sendWalls(walls, data);
    sendUndestroyables(undestrs, data);

    broadcastNotification(nullptr, Notifications::GAME_STATE, data);
}


void QGame::sendPlayersInfo(QVariantMap &data)
{
    QVariantList chs;
    for (QGameCharacter* character : characters.values())
    {
        QVariantMap p;
        p.insert(GameDataObjects::P_NAME, QVariant(character->getPlayer()->getName()));
        p.insert(GameDataObjects::P_BOMBS, QVariant(character->getBombs()));
        p.insert(GameDataObjects::P_DEATHS, QVariant(character->getDeaths()));
        p.insert(GameDataObjects::P_KILLS, QVariant(character->getKills()));
        p.insert(GameDataObjects::P_COORDINATIONS, QJson::QObjectHelper::qobject2qvariant(&character->getCoordinations()));

        chs << p;
    }
    data.insert(GameDataObjects::PLAYER, chs);
}

void QGame::sendFiresInfo(const QList<QGameObject>& list, QVariantMap &data)
{
    QVariantList fires;
    for (const QGameObject& fire : list)
        fires << QJson::QObjectHelper::qobject2qvariant(&fire.getCoords());

    data.insert(GameDataObjects::FIRE, fires);
}

void QGame::sendPowerups(const QList<QGameObject>& list, QVariantMap &data)
{
    QVariantList powerups;
    for (const QGameObject& powerup : list)
        powerups << QJson::QObjectHelper::qobject2qvariant(&powerup.getCoords());

    data.insert(GameDataObjects::POWERUP, powerups);
}

void QGame::sendBombsInfo(const QList<QGameObject> &list, QVariantMap &data)
{
    QVariantList bombs;
    for (const QGameObject& bomb : list)
        bombs << QJson::QObjectHelper::qobject2qvariant(&bomb.getCoords());

    data.insert(GameDataObjects::BOMB, bombs);
}

void QGame::sendWalls(const QList<QGameObject> &list, QVariantMap &data)
{
    QVariantList walls;
    for (const QGameObject& wall : list)
        walls << QJson::QObjectHelper::qobject2qvariant(&wall.getCoords());

    data.insert(GameDataObjects::WALL, walls);
}

void QGame::sendUndestroyables(const QList<QGameObject> &list, QVariantMap &data)
{
    QVariantList undestrs;
    for (const QGameObject& undestr : list)
        undestrs << QJson::QObjectHelper::qobject2qvariant(&undestr.getCoords());

    data.insert(GameDataObjects::UNDESTROYABLE, undestrs);
}

























