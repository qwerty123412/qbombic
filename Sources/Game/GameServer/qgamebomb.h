#ifndef QGAMEBOMB_H
#define QGAMEBOMB_H

#include <qgamecharacter.h>

class QGameBomb
{
public:

    static const unsigned TTL = 2000;// time in ms

    QGameBomb(QGameCharacter* owner, unsigned ttl = 20) :
        owner(owner), ttl(ttl)
    {
    }

    bool explosion() { return !--ttl; }

    // next bomb processing will explose
    void fired() { ttl = 1; }

    QGameCharacter* getOwner() { return owner; }

private:
    QGameCharacter* owner;
    unsigned ttl;
};


#endif // QGAMEBOMB_H
