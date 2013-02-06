#ifndef QCOORDINATIONS_H
#define QCOORDINATIONS_H

#include <QObject>


class QCoordinations : public QObject
{
    Q_OBJECT

    Q_PROPERTY(unsigned x READ getX WRITE setX)
    Q_PROPERTY(unsigned y READ getY WRITE setY)

public:
    QCoordinations(QObject* parent = nullptr) :
        QObject(parent), x(0), y(0)
    {}

    QCoordinations(unsigned x, unsigned y, QObject* parent = nullptr) :
        QObject(parent), x(x), y(y)
    {}

    QCoordinations(const QCoordinations& another) :
        QObject(another.parent()), x(another.x), y(another.y)
    {}


    ~QCoordinations()
    {}

    QCoordinations& operator=(const QCoordinations& another)
    {
        if (this != &another)
        {
            setParent(another.parent());
            x = another.x;
            y = another.y;
        }
        return *this;
    }


    void setX(unsigned _x) { x = _x; }
    void setY(unsigned _y) { y = _y; }

    unsigned getX() const { return x; }
    unsigned getY() const { return y; }

    unsigned& X() { return x; }
    unsigned& Y() { return y; }

    inline bool operator==(const QCoordinations& another) const
    {
        return x == another.x && y == another.y;
    }

    inline bool operator!=(const QCoordinations& another) const
    {
        return !operator==(another);
    }

    inline bool operator<(const QCoordinations& another) const
    {
        return x*y < another.x * another.y;
    }

    inline bool operator>(const QCoordinations& another) const
    {
        return x*y > another.x * another.y;
    }

    inline bool operator<=(const QCoordinations& another) const
    {
        return !operator>(another);
    }

    inline bool operator>=(const QCoordinations& another) const
    {
        return !operator<(another);
    }

private:
    unsigned x;
    unsigned y;

};



#endif // QCOORDINATIONS_H
