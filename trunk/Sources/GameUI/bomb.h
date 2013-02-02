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

    Bomb* set_bomb_ownership( int character_id ) { m_bomb_owner = character_id; return this; }
    int get_bomb_ownership() { return m_bomb_owner; }

    int get_bomb_id() { return m_id; }

    virtual ~Bomb();

signals:
    
public slots:
    
protected:
    int m_id;
    int m_pos_x, m_pos_y;
    int time_to_live;
    int m_bomb_owner;
    bool m_bomb_sent;
    AnimatedPixmapItem* m_bomb;
};

#endif // BOMB_H
