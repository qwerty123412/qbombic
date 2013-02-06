#ifndef QCHARACTER_H
#define QCHARACTER_H

#include <QString>
#include "qcoordinations.h"

class QCharacter
{
public:
    QCharacter()
    {}

    ~QCharacter()
    {}

    const QString& getName() const { return name; }
    void setName(const QString& n) { name = n; }

    const QCoordinations& getCoordinations() const { return coords; }
    void setCoordinations(const QCoordinations& c) { coords = c; }

    unsigned getBombs() const { return bombs; }
    void setBombs(unsigned b) { bombs = b; }

    unsigned getKills() const { return kills; }
    void setKills(unsigned k) { kills = k; }

    unsigned getDeaths() const { return deaths; }
    void setDeaths(unsigned d) { deaths = d; }

private:
    unsigned bombs;
    unsigned kills;
    unsigned deaths;
    QString name;
    QCoordinations coords;
};

#endif // QCHARACTER_H
