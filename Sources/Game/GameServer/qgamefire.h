#ifndef QGAMEFIRE_H
#define QGAMEFIRE_H

#include <qgamecharacter.h>

class QGameFire
{
public:

    static const unsigned TTL = 300;// time in ms

    QGameFire(QGameCharacter* owner, unsigned ttl = 3) :
        owner(owner), ttl(ttl)
    {}

    bool firing() { return --ttl; }

    QGameCharacter* getOwner() { return owner; }
private:
    QGameCharacter* owner;
    unsigned ttl;
};


#endif // QGAMEFIRE_H
