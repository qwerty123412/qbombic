#ifndef QGAMESTATE_H
#define QGAMESTATE_H

#include <QObject>
#include <QPoint>

class QGameState : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int playerPosX READ getPlayerPosX WRITE setPlayerPosX)
    Q_PROPERTY(int playerPosY READ getPlayerPosY WRITE setPlayerPosY)
public:
    explicit QGameState(QObject *parent = 0);

    void setPlayerPosX(int x) { playerPosX = x; }
    int getPlayerPosX() const { return playerPosX; }

    void setPlayerPosY(int y) { playerPosY = y; }
    int getPlayerPosY() const { return playerPosY; }
signals:
    
public slots:
private:
    int playerPosX;
    int playerPosY;
};

#endif // QGAMESTATE_H
