#ifndef CHARACTER_H
#define CHARACTER_H

#include "sprites.h"
#include "bomb.h"

#include <QObject>

class Character : public QObject
{
    Q_OBJECT
public:
    explicit Character(AnimatedPixmapItem *character_sprite, int pos_x, int pos_y, QObject *parent = 0);
    int getX() { return m_pos_x; }
    int getY() { return m_pos_y; }
    int getTargetX() { return m_target_x; }
    int getTargetY() { return m_target_y; }
    int getKills() { return m_kills; }
    int getDeaths() { return m_deaths; }

    AnimatedPixmapItem* getCharacterSprite();

    Character* setXY(int x, int y);
    Character* setTargetXY(int x, int y) { m_target_x = x; m_target_y = y; return this; }
    Character* setKills(int kills) { m_kills = kills; return this; }
    Character* setDeaths(int deaths) { m_deaths = deaths; return this; }

    int getUpperLeftQuadrant(int tile_size, int speed, int level_tiles_x);
    int getLowerLeftQuadrant(int tile_size, int speed, int level_tiles_x);
    int getUpperRightQuadrant(int tile_size, int speed, int level_tiles_x);
    int getLowerRightQuadrant(int tile_size, int speed, int level_tiles_x);

    int get_quadrant_character_center(int tile_size, int level_tiles_x);

    bool can_laid_bomb() { return m_max_bombs > m_bomb_count ? true : false; }
    void bomb_laid() { m_bomb_count++; }
    void bomb_exploded() { m_bomb_count--; }

    void raise_bomb_limit() { m_max_bombs++; }
    void set_bomb_limit(int limit) { m_max_bombs = limit; }    

    int get_id() { return m_id; }
    Character* set_id(int id) { m_id = id; return this; }

    void set_last_bomb(Bomb* bomb) { m_last_bomb = bomb; }
    Bomb* get_last_bomb() { return m_last_bomb; }

signals:
    
public slots:
    
private:
    int m_id;
    int m_pos_x, m_pos_y;
    int m_target_x, m_target_y;
    int m_max_bombs, m_bomb_count;
    int m_kills, m_deaths;
    AnimatedPixmapItem* m_character_sprite;
    Bomb* m_last_bomb;
};

#endif // CHARACTER_H
