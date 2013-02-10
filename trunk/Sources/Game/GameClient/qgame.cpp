#include "qgame.h"

#include "qgamestate.h"
#include "qcharacter.h"
#include "qcoordinations.h"
#include "RequestsResponses.h"

#include<functional>

using namespace std::placeholders;

QGame::QGame(QObject *parent) :
    QObject(parent)
{
    main = static_cast<MainWindow*>(parent);
    view = window.getView();

    window.setWindowTitle("Bombitch " + main->getGameName() + " [" + main->getPlayerName() + "]");
}

QGame::~QGame()
{
    window.close();
    main->getComm()->clearNotification(Notifications::GAME_STATE);
}

void QGame::start()
{
    window.show();

    main->getComm()->registerNotification(Notifications::GAME_STATE, std::bind(&QGame::processState, this, _1));
    connect(&window, SIGNAL(destroyed()), main, SLOT(leaveGame()));
}

void QGame::moveDown()
{
    main->getComm()->sendNotification(GameEvents::MOVE_UP);
}

void QGame::moveLeft()
{
    main->getComm()->sendNotification(GameEvents::MOVE_LEFT);
}

void QGame::moveRight()
{
    main->getComm()->sendNotification(GameEvents::MOVE_RIGHT);
}

void QGame::moveUp()
{
    main->getComm()->sendNotification(GameEvents::MOVE_UP);
}

void QGame::pushBomb()
{
    main->getComm()->sendNotification(GameEvents::PUSH_BOMB);
}

void QGame::processState(const QVariant &data)
{
    QVariantMap map = data.toMap();

    QCoordinations coords;
    QCharacter character;
    QList<QCoordinations> walls, bombs, fires, powerups, undestroyables;// data
    QList<QCharacter> characters;// data
    QCharacter me;// data

    for( QVariant& b : map[GameDataObjects::BOMB].toList() )
    {
        QJson::QObjectHelper::qvariant2qobject(b.toMap(), &coords);
        bombs << coords;
    }

    for( QVariant& f : map[GameDataObjects::FIRE].toList() )
    {
        QJson::QObjectHelper::qvariant2qobject(f.toMap(), &coords);
        fires << coords;
    }

    for( QVariant& p : map[GameDataObjects::POWERUP].toList() )
    {
        QJson::QObjectHelper::qvariant2qobject(p.toMap(), &coords);
        powerups << coords;
    }

    for( QVariant& w : map[GameDataObjects::WALL].toList() )
    {
        QJson::QObjectHelper::qvariant2qobject(w.toMap(), &coords);
        walls << coords;
    }

    for( QVariant& u : map[GameDataObjects::UNDESTROYABLE].toList() )
    {
        QJson::QObjectHelper::qvariant2qobject(u.toMap(), &coords);
        undestroyables << coords;
    }

    for( QVariant& ch : map[GameDataObjects::PLAYER].toList() )
    {
        QVariantMap player = ch.toMap();
        character.setBombs(player[GameDataObjects::P_BOMBS].toUInt());
        character.setDeaths(player[GameDataObjects::P_DEATHS].toUInt());
        character.setKills(player[GameDataObjects::P_KILLS].toUInt());
        QJson::QObjectHelper::qvariant2qobject(player[GameDataObjects::P_COORDINATIONS].toMap(), &coords);
        character.setCoordinations(coords);
        character.setName(player[GameDataObjects::P_NAME].toString());

        if (character.getName() == main->getPlayerName())
            me = character;
        characters << character;
    }

    // send into game area here:
// TODO: implement
}

