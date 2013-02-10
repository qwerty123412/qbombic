#ifndef POWERUP_H
#define POWERUP_H

#include "sprites.h"

#include <QObject>

class Powerup : public QObject
{
    Q_OBJECT
public:
    explicit Powerup(AnimatedPixmapItem *powerup_sprite, QObject *parent = 0);

    Powerup* setXY(int x, int y);
    int getX() { return m_pos_x; }
    int getY() { return m_pos_y; }

    int get_quadrant(int tile_size, int level_tiles_x);

    void activate() { m_activated = true; }
    void deactivate() { m_activated = false; }
    bool is_activated() { return m_activated; }

    virtual ~Powerup();

signals:
    
public slots:
    
private:

    int m_id;
    int m_pos_x, m_pos_y;
    bool m_activated;
    AnimatedPixmapItem* m_powerup_sprite;

};

#endif // POWERUP_H
