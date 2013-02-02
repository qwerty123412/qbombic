#ifndef BOMB_H
#define BOMB_H

#include "sprites.h"

#include <QObject>
#include <string>

class Bomb : public QObject
{
    Q_OBJECT
public:
    explicit Bomb(AnimatedPixmapItem *bomb, int pos_x, int pos_y, int ttl, QObject *parent = 0);
    bool process();
    int getX();
    int getY();
    AnimatedPixmapItem* getBomb();
    void setXY(int x, int y);
    void explode();

    virtual ~Bomb();

signals:
    
public slots:
    
protected:
    int m_pos_x, m_pos_y;
    int time_to_live;
    AnimatedPixmapItem* m_bomb;
};

#endif // BOMB_H
