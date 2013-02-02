#ifndef EXPLOSION_H
#define EXPLOSION_H
#include "bomb.h"

#include <QGraphicsItem>

class Explosion : public Bomb
{
public:
    Explosion(AnimatedPixmapItem *bomb,
              int pos_x, int pos_y,
              QList<Explosion*> &explosions,
              int ttl, int type, int direction, int range, int tile_size,
              std::vector<int>* level_data,
              int level_width,
              QList<Bomb*> *bombs);

    bool check_for_bomb(QList<Bomb*> *bombs);
    Explosion* set_bomb_ownership( int character_id ) { m_bomb_owner = character_id; return this; }

private:
    int m_type;
    int m_direction;
    int m_range;
};

#endif // EXPLOSION_H
