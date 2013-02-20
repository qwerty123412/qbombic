#include "powerup.h"

Powerup::Powerup(AnimatedPixmapItem *powerup_sprite, QObject *parent) :
    QObject(parent)
{
    m_powerup_sprite = powerup_sprite;
}

Powerup* Powerup::setXY(int x, int y) {
    m_pos_x = x;
    m_pos_y = y;
    m_powerup_sprite->setPos(m_pos_x, m_pos_y);

    return this;
}

int Powerup::get_quadrant(int tile_size, int level_tiles_x) {
    return floor((m_pos_x )/tile_size)
            + level_tiles_x * floor((m_pos_y)/tile_size);
}

void Powerup::delete_sprite() {
    m_powerup_sprite->setVisible(false);
}

Powerup::~Powerup()
{
}
