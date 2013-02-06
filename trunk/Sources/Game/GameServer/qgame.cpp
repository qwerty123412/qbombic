#include "qgame.h"

#include "qgameserver.h"
#include <qjson/qobjecthelper.h>
#include "qgameinfo.h"

#include "RequestsResponses.h"
#include "qcoordinations.h"
#include "qgamecharacter.h"

static const unsigned SIZE_X = 26;
static const unsigned SIZE_Y = 18;

static inline unsigned at(unsigned x, unsigned y, unsigned line = SIZE_X)
{
    return y * line + x;
}

static inline void index(unsigned in, unsigned &x, unsigned &y, unsigned line = SIZE_X)
{
    x = in % line;
    y = in / line;
}

static inline unsigned size(unsigned x = SIZE_X, unsigned y = SIZE_Y)
{
    return x * y;
}


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
    unsigned count = 5;
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
        if (area.contains(QCoordinations(x, y)))
            continue;
        area.insert(QCoordinations(x, y, this), QGameObject(new QGameWall(), QCoordinations(x, y, this)));
        --count;
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
            area.insert(center, QGameObject(new QGameFire(bomb->getOwner()), center));

            for (QPair<int,int> way : ways)
            {
                QCoordinations actual(center);
                for (int i = 0; i < 4; ++i)
                {
                    actual.X() += way.first;
                    actual.Y() += way.second;

                    if (!area.contains(actual) || area[actual].type() == QGameObject::POWERUP)
                    {
                        area.insert(actual, QGameObject(new QGameFire(bomb->getOwner()), actual));
                        continue;
                    }
                    QGameObject& current = area[actual];

                    switch(current.type())
                    {
                    case QGameObject::BOMB:
                        current.data<QGameBomb>()->fired();
                    case QGameObject::FIRE:
                        break;
                    case QGameObject::WALL:
                        area.insert(actual, QGameObject(new QGameFire(bomb->getOwner()), actual));
                        break;
                    }
                    if (players.contains(actual))
                    {
                        dead.push_back(players[actual]);
                        players.remove(actual);
                        bomb->getOwner()->kill();
                        area.insert(actual, QGameObject(new QGameFire(bomb->getOwner()), actual));
                        continue;
                    }
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
            area.insert(character->getCoordinations(), QGameObject(new QGameBomb(character), character->getCoordinations()));
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

        if (players.contains(newCoords))// place is full
            continue;
        if (area.contains(newCoords) && area[newCoords].type() == QGameObject::WALL)// there is a wall
            continue;

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
            if (!area.contains(QCoordinations(x, y)) && players.contains(QCoordinations(x, y)))
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
    sendPlayersInfo(data);
    sendFiresInfo(data);
    sendBombsInfo(data);
    sendPowerups(data);
    sendWalls(data);

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

void QGame::sendFiresInfo(QVariantMap &data)
{
    QVariantList fires;
    for (QGameObject& fire : area.values())
    {
        if (fire.type() != QGameObject::FIRE)
            continue;

        fires << QJson::QObjectHelper::qobject2qvariant(&fire.getCoords());
    }
    data.insert(GameDataObjects::FIRE, fires);
}

void QGame::sendPowerups(QVariantMap &data)
{
    QVariantList powerups;
    for (QGameObject& powerup : area.values())
    {
        if (powerup.type() != QGameObject::POWERUP)
            continue;

        powerups << QJson::QObjectHelper::qobject2qvariant(&powerup.getCoords());
    }
    data.insert(GameDataObjects::POWERUP, powerups);
}

void QGame::sendBombsInfo(QVariantMap &data)
{
    QVariantList bombs;
    for (QGameObject& bomb : area.values())
    {
        if (bomb.type() != QGameObject::BOMB)
            continue;

        bombs << QJson::QObjectHelper::qobject2qvariant(&bomb.getCoords());
    }
    data.insert(GameDataObjects::BOMB, bombs);
}

void QGame::sendWalls(QVariantMap &data)
{
    QVariantList walls;
    for (QGameObject& wall : area.values())
    {
        if (wall.type() != QGameObject::WALL)
            continue;

        walls << QJson::QObjectHelper::qobject2qvariant(&wall.getCoords());
    }
    data.insert(GameDataObjects::WALL, walls);
}

























