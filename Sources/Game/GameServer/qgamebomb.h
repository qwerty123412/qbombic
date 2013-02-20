#ifndef QGAMEBOMB_H
#define QGAMEBOMB_H

#include <qgamecharacter.h>
#include "qgamesettings.h"

class QGameBomb
{
public:

    static const unsigned TTL = 2000;// time in ms

    QGameBomb(QGameCharacter* owner, unsigned ttl = time2frame<QGameBomb>()) :
        owner(owner), ttl(ttl)
    {
    }

    bool explosion() { return !--ttl; }

    QGameCharacter* getOwner() { return owner; }

private:
    QGameCharacter* owner;
    unsigned ttl;
};


#endif // QGAMEBOMB_H
