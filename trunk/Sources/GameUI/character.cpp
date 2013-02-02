#include "character.h"

Character::Character(AnimatedPixmapItem *character_sprite, int pos_x, int pos_y, QObject *parent) :
    QObject(parent)
{
    m_character_sprite = character_sprite;
    m_character_sprite->setPos(pos_x, pos_y);
    m_pos_x = pos_x;
    m_pos_y = pos_y;
    m_max_bombs = 3;
    m_bomb_count = 0;
}

int Character::getX() {
    return m_pos_x;
}

int Character::getY() {
    return m_pos_y;
}

void Character::setXY(int x, int y) {
    m_pos_x = x;
    m_pos_y = y;
    m_character_sprite->setPos(m_pos_x, m_pos_y);
}

AnimatedPixmapItem* Character::getCharacterSprite() {
    return m_character_sprite;
}

int Character::getUpperLeftQuadrant(int tile_size, int speed, int level_tiles_x) {
    return floor( (m_pos_x + speed) / tile_size) + level_tiles_x * floor( (m_pos_y - speed) / tile_size);
}

int Character::getUpperRightQuadrant(int tile_size, int speed, int level_tiles_x) {
    return floor( (m_pos_x + m_character_sprite->boundingRect().width() - speed ) / tile_size)
            + level_tiles_x * floor( (m_pos_y - speed) / tile_size);
}

int Character::getLowerLeftQuadrant(int tile_size, int speed, int level_tiles_x) {
    return floor( (m_pos_x + speed) / tile_size)
            + level_tiles_x * floor(( m_pos_y + m_character_sprite->boundingRect().height() + speed)/tile_size);
}

int Character::getLowerRightQuadrant(int tile_size, int speed, int level_tiles_x) {
    return floor((m_pos_x + m_character_sprite->boundingRect().width() - speed )/tile_size)
            + level_tiles_x * floor((m_pos_y + m_character_sprite->boundingRect().height() + speed)/tile_size);
}

int Character::get_quadrant_character_center(int tile_size, int level_tiles_x) {
    return floor((m_pos_x + m_character_sprite->boundingRect().width()/2 )/tile_size)
            + level_tiles_x * floor((m_pos_y + m_character_sprite->boundingRect().height()/2)/tile_size);
}
