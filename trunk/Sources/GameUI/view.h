/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

/*
 * KAsteroids - Copyright (c) Martin R. Jones 1997
 *
 * Part of the KDE project
 */

#ifndef __AST_VIEW_H__
#define __AST_VIEW_H__

#include <QWidget>
#include <QList>
#include <QMultiHash>
#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimerEvent>
#include <QShowEvent>
#include <QResizeEvent>
#include "sprites.h"

#include "bomb.h"
#include "explosion.h"
#include "character.h"
#include "powerup.h"
#include "block.h"

#define MAX_POWER_LEVEL          1000

class QGame;

class KAsteroidsView : public QWidget
{
    Q_OBJECT
public:
    KAsteroidsView( QWidget *parent = 0);
    virtual ~KAsteroidsView();

    int refreshRate;

    void reset();        
    void newGame();
    void endGame();
    void newPlayer();

    void goUp( bool r ) { mGoUp = r; }
    void goDown( bool r ) { mGoDown = r; }
    void goLeft( bool r ) { mGoLeft = r; }
    void goRight( bool r ) { mGoRight = r; }    

    //new
    void bomb( bool s ) { bombingEnabled = s; }
    bool check_bomb_placement_possibility(int pos_x, int pos_y);
    bool check_explosion_placement_possibility(int pos_x, int pos_y);
    //end

    void pause( bool p);

    void showText( const QString &text, const QColor &color, bool scroll=TRUE );
    void hideText();

    Character* getPlayer() const { return m_player; }
    const QList<Character*>& getPlayers() const { return m_players; }

    void set_qgame(QGame* client_object) { m_client_object = client_object; }

    void setPlayerCoordsXY(int x, int y);
    bool check_wall_count(int count) { return m_blocks.size() == count; }

    void addUndestroyable(int x, int y);
    void addWall(int x, int y);
    void addBomb(int x, int y);
    void addExplosion(int x, int y);
    void addPowerup(int x, int y);

    void updatePlayer(QString name, int x, int y, int kills, int deaths);

    void center_on(QString name);

    void blocks_deactivate();
    void delete_deactivated_blocks();
    void bombs_deactivate();
    void delete_deactivated_bombs();
    void powerups_deactivate();
    void delete_deactivated_powerups();
    void explosions_deactivate();
    void delete_deactivated_explosions();

signals:
    void shipKilled();
    void rockHit( int size );
    void rocksRemoved();
    void updateVitals();

    void move(int direction); // 0 = up, 1 = right, 2 = down, 3 = left
    void bombLaid();

    void killCount(int kills);
    void deathCount(int deaths);

private slots:    

protected:
    bool readSprites();
    void wrapSprite( QGraphicsItem * );

    void processChar();
    void processBombs();
    void processDeaths();
    void processPowerups();
    void processBlocks();

    //void receiveBombs();

    Character* create_character();
    Powerup* create_powerup();
    Block* create_block();

    double randDouble();
    int randInt( int range );

    virtual void resizeEvent( QResizeEvent *event );
    virtual void timerEvent( QTimerEvent * );

    void showEvent( QShowEvent * );

private:    
    QGame* m_client_object;

    QGraphicsScene field;
    QGraphicsView view;
    QMap<int, QList<QPixmap> > animation;

    QMap<QString, Character*> players;

    QGraphicsTextItem *textSprite;

    //new
    AnimatedPixmapItem* m_bomb;
    AnimatedPixmapItem* m_explosion;
    std::vector<int> m_level_data;

    QList<Bomb*> m_bombs;
    QList<Explosion*> m_explosions;
    QList<Character*> m_players;
    QList<Powerup*> m_powerups;
    QList<Block*> m_blocks;

    Character* m_player;
    QString m_player_name;

    int m_bomb_time_left;
    //end

    bool mGoUp;
    bool mGoDown;
    bool mGoLeft;
    bool mGoRight;    
    bool bombingEnabled;

    int bombingCooldown;
    int upCool, downCool, leftCool, rightCool;

    int  textDy;
    int  mFrameNum;
    bool mPaused;
    int  mTimerId;

    QTimer *shieldTimer;
    bool initialized;
};

#endif
