#ifndef QGAMESTATE_H
#define QGAMESTATE_H

#include <QObject>
#include <QPoint>

class QGameState : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int playerPosX READ getPlayerPosX WRITE setPlayerPosX)
    Q_PROPERTY(int playerPosY READ getPlayerPosY WRITE setPlayerPosY)
    Q_PROPERTY(QString playerName READ getPlayerName WRITE setPlayerName)
public:
    explicit QGameState(QObject *parent = 0);



    void setPlayerPosX(int x) { playerPosX = x; }
    int getPlayerPosX() const { return playerPosX; }

    void setPlayerPosY(int y) { playerPosY = y; }
    int getPlayerPosY() const { return playerPosY; }

    void setPlayerName(const QString& name) { playerName = name; }
    const QString& getPlayerName() const { return playerName; }


signals:
    
public slots:
private:
    int playerPosX;
    int playerPosY;
    QString playerName;
};

#endif // QGAMESTATE_H
