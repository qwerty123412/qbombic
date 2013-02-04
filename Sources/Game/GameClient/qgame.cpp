#include "qgame.h"

#include "qgamestate.h"
#include "RequestsResponses.h"

QGame::QGame(QObject *parent) :
    QObject(parent)
{
    main = static_cast<MainWindow*>(parent);
    view = window.getView();
    connect(&sendTimer, SIGNAL(timeout()), SLOT(onTick()));

    main->getComm()->registerNotification(Notifications::GAME_STATE, [this](const QVariant& message)
    {
            QGameState state;
            QJson::QObjectHelper::qvariant2qobject(message.toMap(), &state);
            view->getPlayers().last()->setXY(state.getPlayerPosX(), state.getPlayerPosY());
    });
    connect(&window, SIGNAL(destroyed()), main, SLOT(leaveGame()));

    window.setWindowTitle("Bombi " + main->getGameName() + " [" + main->getPlayerName() + "]");
}

QGame::~QGame()
{
    window.close();
    sendTimer.stop();
    main->getComm()->clearNotification(Notifications::GAME_STATE);
}

void QGame::start()
{
    window.show();

    view->newPlayer();

    sendTimer.start(50);
}


void QGame::onTick()
{
    QGameState state;
    state.setPlayerPosX(view->getPlayer()->getX());
    state.setPlayerPosY(view->getPlayer()->getY());

    QVariant map = QJson::QObjectHelper::qobject2qvariant(&state);

    main->getComm()->sendNotification(Notifications::GAME_STATE, map);
}
