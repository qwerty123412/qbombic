#include "explosion.h"

Explosion::Explosion(AnimatedPixmapItem *bomb,
                     int pos_x, int pos_y,
                     QList<Explosion*> &explosions,
                     int ttl, int type, int direction, int range, int tile_size,
                     std::vector<int>* level_data, int level_width,
                     QList<Bomb*> *bombs) : Bomb(bomb, pos_x, pos_y, ttl)
{
    m_pos_x = pos_x;
    m_pos_y = pos_y;
    if(check_for_bomb(bombs)) {
        return;
    }

    if(direction == 0) {
        if((*level_data)[ floor(pos_x/tile_size) + level_width * floor((pos_y - tile_size)/tile_size) ] == 0)
            explosions.push_back(new Explosion(new AnimatedPixmapItem(*bomb),
                                   pos_x,
                                   pos_y - tile_size,
                                   explosions,
                                   ttl, type, 1, range-1, tile_size, level_data, level_width, bombs));

        if((*level_data)[ floor( (pos_x + tile_size)/tile_size) + level_width * floor((pos_y)/tile_size) ] == 0)
            explosions.push_back(new Explosion(new AnimatedPixmapItem(*bomb),
                                   pos_x + tile_size,
                                   pos_y,
                                   explosions,
                                   ttl, type, 2, range-1, tile_size, level_data, level_width, bombs));

        if((*level_data)[ floor( (pos_x) /tile_size) + level_width * floor((pos_y + tile_size)/tile_size) ] == 0)
            explosions.push_back(new Explosion(new AnimatedPixmapItem(*bomb),
                                   pos_x,
                                   pos_y + tile_size,
                                   explosions,
                                   ttl, type, 3, range-1, tile_size, level_data, level_width, bombs));

        if((*level_data)[ floor( (pos_x - tile_size)/tile_size) + level_width * floor((pos_y)/tile_size) ] == 0)
            explosions.push_back(new Explosion(new AnimatedPixmapItem(*bomb),
                                   pos_x - tile_size,
                                   pos_y,
                                   explosions,
                                   ttl, type, 4, range-1, tile_size, level_data, level_width, bombs));
    } else if(direction == 1 && range > 0) {
        if((*level_data)[ floor(pos_x/tile_size) + level_width * floor((pos_y - tile_size)/tile_size) ] == 0)
            explosions.push_back(new Explosion(new AnimatedPixmapItem(*bomb),
                                   pos_x,
                                   pos_y - tile_size,
                                   explosions,
                                   ttl, type, direction, range-1, tile_size, level_data, level_width, bombs));
    } else if(direction == 2 && range > 0) {
        if((*level_data)[ floor( (pos_x + tile_size)/tile_size) + level_width * floor((pos_y)/tile_size) ] == 0)
            explosions.push_back(new Explosion(new AnimatedPixmapItem(*bomb),
                                   pos_x + tile_size,
                                   pos_y,
                                   explosions,
                                   ttl, type, 2, range-1, tile_size, level_data, level_width, bombs));
    } else if(direction == 3 && range > 0) {
        if((*level_data)[ floor( (pos_x) /tile_size) + level_width * floor((pos_y + tile_size)/tile_size) ] == 0)
            explosions.push_back(new Explosion(new AnimatedPixmapItem(*bomb),
                                   pos_x,
                                   pos_y + tile_size,
                                   explosions,
                                   ttl, type, 3, range-1, tile_size, level_data, level_width, bombs));
    } else if(direction == 4 && range > 0) {
        if((*level_data)[ floor( (pos_x - tile_size)/tile_size) + level_width * floor((pos_y)/tile_size) ] == 0)
            explosions.push_back(new Explosion(new AnimatedPixmapItem(*bomb),
                                   pos_x - tile_size,
                                   pos_y,
                                   explosions,
                                   ttl, type, 4, range-1, tile_size, level_data, level_width, bombs));
    }
}

bool Explosion::check_for_bomb(QList<Bomb *> *bombs) {
    QList<Bomb*>::Iterator bomb_it;
    for(bomb_it = bombs->begin(); bomb_it != bombs->end(); bomb_it++) {
        if((*bomb_it)->getX() == m_pos_x && (*bomb_it)->getY() == m_pos_y) {
            (*bomb_it)->explode();
            return true;
        }
    }
    return false;
}
