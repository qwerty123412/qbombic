#include "bomb.h"

Bomb::Bomb(AnimatedPixmapItem *bomb, int pos_x, int pos_y, int ttl, QObject *parent) :
    QObject(parent)
{
    m_bomb = bomb;
    m_bomb->setPos(pos_x, pos_y);
    time_to_live = ttl;
    m_pos_x = pos_x;
    m_pos_y = pos_y;
    m_bomb_owner = false;
    m_bomb_sent = false;
    m_id = qrand() % 10000;
}

bool Bomb::process() {

    if(time_to_live-- < 1) {
        m_bomb->setFrame(1);
        return true;
    }
    m_bomb->setFrame(time_to_live/5 == 1 ? 2 : time_to_live/5);
    return false;
}

int Bomb::getX() {
    return m_pos_x;
}

int Bomb::getY() {
    return m_pos_y;
}

void Bomb::setXY(int x, int y) {
    m_pos_x = x;
    m_pos_y = y;
    m_bomb->setPos(x, y);
}

AnimatedPixmapItem* Bomb::getBomb() {
    return m_bomb;
}

void Bomb::explode() {
    time_to_live = 0;
}

int Bomb::get_quadrant(int tile_size, int level_tiles_x) {
    return floor((m_pos_x )/tile_size)
            + level_tiles_x * floor((m_pos_y)/tile_size);
}

Bomb::~Bomb()
{
    delete m_bomb;
}
