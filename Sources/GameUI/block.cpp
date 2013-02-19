#include "block.h"

Block::Block(AnimatedPixmapItem *block_sprite, QObject *parent) :
    QObject(parent)
{
    m_block_sprite = block_sprite;
}

Block* Block::setXY(int x, int y) {
    m_pos_x = x;
    m_pos_y = y;
    m_block_sprite->setPos(m_pos_x, m_pos_y);

    return this;
}

int Block::get_quadrant(int tile_size, int level_tiles_x) {
    return floor((m_pos_x )/tile_size)
            + level_tiles_x * floor((m_pos_y)/tile_size);
}

void Block::delete_sprite() {
    m_block_sprite->setVisible(false);
}

Block::~Block()
{
    //delete m_block_sprite;
}
