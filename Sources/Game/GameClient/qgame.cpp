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

    view->set_qgame(this);
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
    connect(&window, SIGNAL(move(int)), this, SLOT(processMovements(int)));
    connect(&window, SIGNAL(bombLaid()), this, SLOT(bombLaid()));
}

void QGame::processMovements(int direction)
{
    if(direction == 0) {
        moveUp();
        qDebug() << "up";
    } else if(direction == 3) {
        moveLeft();
        qDebug() << "left";
    } else if(direction == 2) {        
        moveDown();
        qDebug() << "down";
    } else if(direction == 1) {
        moveRight();
        qDebug() << "right";
    }
}

void QGame::bombLaid() {
    pushBomb();
}

void QGame::moveDown()
{
    main->getComm()->sendNotification(GameEvents::MOVE_DOWN);
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

    for( QCharacter _character : characters ) {
        view->updatePlayer(_character.getName(), _character.getCoordinations().getX(), _character.getCoordinations().getY(), _character.getBombs(), _character.getKills());
    }
    view->center_on(me.getName());

    view->blocks_deactivate();
    for( QCoordinations wall : walls ) {
        view->addWall( wall.getX(), wall.getY() );
    }
    view->delete_deactivated_blocks();

    view->bombs_deactivate();
    for( QCoordinations bomb : bombs ) {
        view->addBomb( bomb.getX(), bomb.getY() );
    }
    view->delete_deactivated_bombs();

    view->explosions_deactivate();
    for( QCoordinations explosion : fires ) {
        view->addExplosion( explosion.getX(), explosion.getY() );
    }
    view->delete_deactivated_explosions();

    view->powerups_deactivate();
    for( QCoordinations powerup : powerups ) {
        view->addPowerup( powerup.getX(), powerup.getY() );
    }
    view->delete_deactivated_powerups();

    for( QCoordinations undestroyable : undestroyables ) {
        view->addUndestroyable( undestroyable.getX(), undestroyable.getY() );
    }

}

