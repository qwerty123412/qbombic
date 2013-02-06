#ifndef QGAMEFIRE_H
#define QGAMEFIRE_H

#include <qgamecharacter.h>

class QGameFire
{
public:
    QGameFire(QGameCharacter* owner, unsigned ttl = 3) :
        owner(owner), ttl(ttl)
    {}

    bool firing() { return --ttl; }
private:
    QGameCharacter* owner;
    unsigned ttl;
};


#endif // QGAMEFIRE_H
