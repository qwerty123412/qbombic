#ifndef BLOCK_H
#define BLOCK_H

#include "sprites.h"

#include <QObject>

class Block : public QObject
{
    Q_OBJECT
public:
    explicit Block(AnimatedPixmapItem *block_sprite, QObject *parent = 0);

    Block* setXY(int x, int y);
    int getX() { return m_pos_x; }
    int getY() { return m_pos_y; }

    int get_quadrant(int tile_size, int level_tiles_x);

    void activate() { m_activated = true; }
    void deactivate() { m_activated = false; }
    bool is_activated() { return m_activated; }

    void delete_sprite();

    virtual ~Block();

signals:
    
public slots:
    
private:

    int m_id;
    int m_pos_x, m_pos_y;
    bool m_activated;
    AnimatedPixmapItem* m_block_sprite;

};

#endif // BLOCK_H
