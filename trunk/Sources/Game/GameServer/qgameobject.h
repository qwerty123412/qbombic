#ifndef QGAMEOBJECT_H
#define QGAMEOBJECT_H

#include <memory>

#include "qgamebomb.h"
#include "qgamefire.h"
#include "qgamewall.h"
#include "qgamepowerup.h"
#include "qcoordinations.h"

class QGameObject
{
public:

    QGameObject() :
        object(nullptr), objType(NONE), coords()
    {}

    QGameObject(const QGameObject&) = default;

    QGameObject(QGameBomb* bomb, const QCoordinations& coords) :
        object(bomb), objType(BOMB), coords(coords)
    {}

    QGameObject(QGameFire* fire, const QCoordinations& coords) :
        object(fire), objType(FIRE), coords(coords)
    {}

    QGameObject(QGameWall* wall, const QCoordinations& coords) :
        object(wall), objType(WALL), coords(coords)
    {}

    QGameObject(QGamePowerUp* power, const QCoordinations& coords) :
        object(power), objType(POWERUP), coords(coords)
    {}

    ~QGameObject()
    {
    }

    QGameObject& operator=(const QGameObject&) = default;

    template< typename result >
    result* data() { return static_cast<result*>(object.get()); }

    int type() const { return objType; }
    const QCoordinations& getCoords() const { return coords; }

    enum Type
    {
        BOMB,
        FIRE,
        WALL,
        POWERUP,
        NONE
    };


    bool operator==(const QGameObject& another) const
    {
        return objType == another.objType && coords == another.coords && object == another.object;
    }
    bool operator!=(const QGameObject& another) const
    {
        return !operator==(another);
    }

    bool operator<(const QGameObject& another) const
    {
        return object.get() < another.object.get();
    }

    bool operator>(const QGameObject& another) const
    {
        return object.get() > another.object.get();
    }

private:

    std::shared_ptr<void> object;
    Type objType;
    QCoordinations coords;
};


#endif // QGAMEOBJECT_H
