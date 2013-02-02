#ifndef CHARACTER_H
#define CHARACTER_H

#include "sprites.h"

#include <QObject>

class Character : public QObject
{
    Q_OBJECT
public:
    explicit Character(AnimatedPixmapItem *character_sprite, int pos_x, int pos_y, QObject *parent = 0);
    int getX();
    int getY();
    AnimatedPixmapItem* getCharacterSprite();
    void setXY(int x, int y);

    int getUpperLeftQuadrant(int tile_size, int speed, int level_tiles_x);
    int getLowerLeftQuadrant(int tile_size, int speed, int level_tiles_x);
    int getUpperRightQuadrant(int tile_size, int speed, int level_tiles_x);
    int getLowerRightQuadrant(int tile_size, int speed, int level_tiles_x);

    bool can_laid_bomb() { return m_max_bombs > m_bomb_count ? true : false; }
    void bomb_laid() { m_bomb_count++; }
    void bomb_exploded() { m_bomb_count--; }
    void set_bomb_limit(int limit) { m_max_bombs = limit; }

signals:
    
public slots:
    
private:
    int m_pos_x, m_pos_y;
    int m_max_bombs, m_bomb_count;
    AnimatedPixmapItem* m_character_sprite;
};

#endif // CHARACTER_H