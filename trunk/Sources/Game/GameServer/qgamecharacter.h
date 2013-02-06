#ifndef QGAMECHARACTER_H
#define QGAMECHARACTER_H

#include <QObject>

#include "qplayer.h"
#include "qcoordinations.h"

class QGameCharacter
{
public:
    QGameCharacter() :
        player(nullptr), coords(), bombs(), deaths(), kills()
    {}

    QGameCharacter(QPlayer* player, const QCoordinations& coords) :
        player(player), coords(coords), bombs(3), deaths(0), kills(0)
    {}

    ~QGameCharacter()
    {}

    QGameCharacter(QGameCharacter&&) = default;
    QGameCharacter(const QGameCharacter&) = default;
    QGameCharacter& operator=(const QGameCharacter&) = default;


    bool putBomb()
    {
        if (!bombs)
            return false;
        --bombs;
        return true;
    }

    void addBomb() { ++bombs; }
    unsigned getBombs() const { return bombs; }

    void die() { ++deaths; }
    unsigned getDeaths() const { return deaths; }

    void kill() { ++kills; }
    unsigned getKills() const { return kills; }


    bool valid() const { return player; }
    QPlayer* getPlayer() const { return player; }

    QCoordinations& getCoordinations() { return coords; }

private:
    QPlayer* player;
    QCoordinations coords;
    unsigned bombs;
    unsigned deaths;
    unsigned kills;
};


#endif // QGAMECHARACTER_H
