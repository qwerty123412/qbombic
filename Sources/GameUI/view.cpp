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

#include <stdlib.h>
#include <math.h>
#include <qapplication.h>
#include <qnamespace.h>
#include <QAction>
#include <QMessageBox>
#include <QScrollArea>
#include <QDir>
#include <QGraphicsItem>
#include <QTimerEvent>
#include <QPixmap>
#include <QResizeEvent>
#include <QShowEvent>
#include <QtDebug>

#include <iostream>

#include <iterator>
#include <algorithm>
#include <array>

#include "view.h"

#define IMG_BACKGROUND QDir::currentPath() + "/../GameUI/sprites/grass_smaller.png"

#define FPS           60
#define REFRESH_DELAY           1000/FPS

#define SHIP_SPEED              0.3
#define MISSILE_SPEED           10.0
#define SHIP_STEPS              64
#define ROTATE_RATE             2
#define SHIELD_ON_COST          1
#define SHIELD_HIT_COST         30
#define BRAKE_ON_COST           4

#define MAX_ROCK_SPEED          2.5
#define MAX_POWERUP_SPEED       1.5
#define MAX_SHIP_SPEED		12
#define MAX_BRAKES              5
#define MAX_SHIELDS             5
#define MAX_FIREPOWER		5

#define TEXT_SPEED              4

#define MOVEMENT_SPEED          5
//#define TILE_SIZE               69
#define TILE_SIZE               48
#define LEVEL_TILES_X           26
#define LEVEL_TILES_Y           18

#define PI_X_2                  6.283185307
#ifndef M_PI
#define M_PI 3.141592654
#endif

static struct
{
    int id;
    const char *path;
    int frames;
}
kas_animations [] =
{    
    { B_WALL, "wall_sprite_smaller.png", 1 },
    { B_BOMB, "bomb_smaller%1.png", 4 },
    { B_EXPLOSION, "explosion_smaller%1.png", 2 },
    { B_PINGU, "pingu.png", 1 },
    { 0,                   0,                          0 }
};

KAsteroidsView::KAsteroidsView( QWidget *parent)
    : QWidget( parent),      
      field(0, 0, LEVEL_TILES_X*TILE_SIZE, LEVEL_TILES_Y*TILE_SIZE),
      view(&field, this)
{    

    m_level_data = std::vector<int>(LEVEL_TILES_X*LEVEL_TILES_Y, 0); // pocet prvku, hodnota
    for(int i = 0; i < LEVEL_TILES_X; i++) {
        m_level_data[i] = B_WALL;
        m_level_data[LEVEL_TILES_X*LEVEL_TILES_Y - LEVEL_TILES_X + i] = B_WALL;
    }
    for(int i = 0; i < LEVEL_TILES_Y; i++) {
        m_level_data[LEVEL_TILES_X*i] = B_WALL;
        m_level_data[LEVEL_TILES_X*i + LEVEL_TILES_X - 1] = B_WALL;
    }
    m_level_data[6*26+11] = B_WALL;
    m_level_data[6*26+12] = B_WALL;
    m_level_data[6*26+13] = B_WALL;
    m_level_data[6*26+15] = B_WALL;
    m_level_data[8*26+11] = B_WALL;
    m_level_data[8*26+13] = B_WALL;
    m_level_data[8*26+15] = B_WALL;
    m_level_data[10*26+11] = B_WALL;
    m_level_data[10*26+13] = B_WALL;
    m_level_data[10*26+15] = B_WALL;

    view.setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    view.setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );    
    view.setCacheMode(QGraphicsView::CacheBackground);
    view.setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    view.setOptimizationFlags(QGraphicsView::DontAdjustForAntialiasing);
    view.viewport()->setFocusProxy( this );

    // velikost levelu
    view.setSceneRect(0, 0, TILE_SIZE*LEVEL_TILES_X, TILE_SIZE*LEVEL_TILES_Y);
    view.centerOn(300, 250);

    QPixmap pm( IMG_BACKGROUND );
    field.setBackgroundBrush( pm );

    textSprite = new QGraphicsTextItem( 0, &field );
    QFont font( "helvetica", 18 );
    textSprite->setFont( font );
    textSprite->setCacheMode(QGraphicsItem::DeviceCoordinateCache);

    refreshRate = REFRESH_DELAY;
    bombingEnabled = FALSE;

    initialized = readSprites();

    mTimerId = -1;
    mPaused = TRUE;

    if ( !initialized ) {
	textSprite->setHtml( tr("<font color=red>Error: Cannot read sprite images</font>") );
	textSprite->setPos( (field.width()-textSprite->boundingRect().width()) / 2,
			    (field.height()-textSprite->boundingRect().height()) / 2 );
    }
}

KAsteroidsView::~KAsteroidsView()
{    
    qDeleteAll(rocks);     rocks.clear();
    qDeleteAll(missiles);  missiles.clear();
    qDeleteAll(bits);      bits.clear();
    qDeleteAll(powerups);  powerups.clear();
    qDeleteAll(exhaust);   exhaust.clear();
}

void KAsteroidsView::reset()
{
    if ( !initialized )
	return;    
    qDeleteAll(rocks);      rocks.clear();
    qDeleteAll(missiles);   missiles.clear();
    qDeleteAll(bits);       bits.clear();
    qDeleteAll(powerups);   powerups.clear();
    qDeleteAll(exhaust);    exhaust.clear();

    mFrameNum = 0;
    mPaused = FALSE;

    ship->hide();   
}

// - --

void KAsteroidsView::newGame()
{
    if ( !initialized )
	return;

    reset();
    if ( mTimerId < 0 )
	mTimerId = startTimer( REFRESH_DELAY );
    emit updateVitals();
}

// - - -

void KAsteroidsView::endGame()
{
    qDeleteAll(rocks);     rocks.clear();
    qDeleteAll(missiles);  missiles.clear();
    qDeleteAll(bits);      bits.clear();
    qDeleteAll(powerups);  powerups.clear();
    qDeleteAll(exhaust);   exhaust.clear();
}

void KAsteroidsView::pause( bool p )
{
    if ( !initialized )
	return;
    if ( !mPaused && p ) {
	if ( mTimerId >= 0 ) {
	    killTimer( mTimerId );
	    mTimerId = -1;
	}
    } else if ( mPaused && !p )
	mTimerId = startTimer( REFRESH_DELAY );
    mPaused = p;
}

// - - -

void KAsteroidsView::newPlayer()
{
    if ( !initialized )
	return;

    ship->setPos( TILE_SIZE*1, TILE_SIZE*1 );
    ship->setFrame( 0 );

    mGoUp = false;
    mGoDown = false;
    mGoLeft = false;
    mGoRight = false;

    //new
    bombingEnabled = FALSE;
    maxBombs = 10;
    bombCount = 0;
    bombCooldown = 0;
    //end

    ship->show();
}

bool KAsteroidsView::readSprites()
{        
    QString sprites_prefix = QDir::currentPath() + "/../GameUI/sprites/";

    std::cout << QDir::currentPath().toStdString() << std::endl;

    int i = 0;
    while ( kas_animations[i].id )
    {
        QList<QPixmap> anim;
        QString wildcard = sprites_prefix + kas_animations[i].path;
        wildcard.replace("%1", "*");
        QFileInfo fi(wildcard);
        foreach (QString entry, QDir(fi.path(), fi.fileName()).entryList())
            anim << QPixmap(fi.path() + "/" + entry);
	animation.insert( kas_animations[i].id, anim );
	i++;
    }

    for(int i = 0; i < m_level_data.size(); i++) {
        if(m_level_data[i] != 0) {
            AnimatedPixmapItem* test = new AnimatedPixmapItem( animation[m_level_data[i]], &field );
            test->setPos((i%LEVEL_TILES_X)*TILE_SIZE, floor(i/LEVEL_TILES_X)*TILE_SIZE);
        }
    }

    m_player = new Character(new AnimatedPixmapItem( animation[B_PINGU], &field ), 2*TILE_SIZE, 2*TILE_SIZE);
    m_players.push_back(m_player);

    ship = new AnimatedPixmapItem( animation[B_PINGU], &field );
    ship->hide();    

    return (!ship->image(0).isNull());
}

void KAsteroidsView::showText( const QString &text, const QColor &color, bool scroll )
{
    if ( !initialized )
	return;
    textSprite->setHtml( QString("<font color=#%1%2%3>%4</font>")
                         .arg(color.red(), 2, 16, QLatin1Char('0'))
                         .arg(color.green(), 2, 16, QLatin1Char('0'))
                         .arg(color.blue(), 2, 16, QLatin1Char('0'))
                         .arg(text) );
    Q_UNUSED(color);
    // ### Porting: no such thing textSprite->setColor( color );

    if ( scroll ) {
	textSprite->setPos( (field.width()-textSprite->boundingRect().width()) / 2,
			    -textSprite->boundingRect().height() );
	textDy = TEXT_SPEED;
    } else {
	textSprite->setPos( (field.width()-textSprite->boundingRect().width()) / 2,
                            (field.height()-textSprite->boundingRect().height()) / 2 );
	textDy = 0;
    }
    textSprite->show();
}

// - - -

void KAsteroidsView::hideText()
{
    textDy = -TEXT_SPEED;
}

// - - -

void KAsteroidsView::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    field.setSceneRect(0, 0, width()-4, height()-4);
    view.resize(width(),height());
}

// - - -

void KAsteroidsView::timerEvent( QTimerEvent * )
{
    field.advance();

    processChar();
    processBombs();

    if ( textSprite->isVisible() )
    {
	if ( textDy < 0 &&
         textSprite->boundingRect().y() <= -textSprite->boundingRect().height() ) {
	    textSprite->hide();
	} else {
	    textSprite->moveBy( 0, textDy );
	}

	if ( textSprite->sceneBoundingRect().y() > (field.height()-textSprite->boundingRect().height())/2 )
	    textDy = 0;
    }

    /*
    if ( vitalsChanged && !(mFrameNum % 10) ) {
	emit updateVitals();
	vitalsChanged = FALSE;
    }
    */

    mFrameNum++;
}

void KAsteroidsView::wrapSprite( QGraphicsItem *s )
{
    int x = int(s->x() + s->boundingRect().width() / 2);
    int y = int(s->y() + s->boundingRect().height() / 2);

    if ( x > field.width() )
	s->setPos( s->x() - field.width(), s->y() );
    else if ( x < 0 )
	s->setPos( field.width() + s->x(), s->y() );

    if ( y > field.height() )
	s->setPos( s->x(), s->y() - field.height() );
    else if ( y < 0 )
	s->setPos( s->x(), field.height() + s->y() );
}

bool KAsteroidsView::check_bomb_placement_possibility(int pos_x, int pos_y) {
    QList<Bomb*>::Iterator it;
    for(it = m_bombs.begin(); it != m_bombs.end(); it++) {
        if((*it)->getX() == pos_x && (*it)->getY() == pos_y) {
            return false;
        }
    }
    return true;
}

bool KAsteroidsView::check_explosion_placement_possibility(int pos_x, int pos_y) {
    QList<Explosion*>::Iterator it;
    for(it = m_explosions.begin(); it != m_explosions.end(); it++) {
        if((*it)->getX() == pos_x && (*it)->getY() == pos_y) {
            return false;
        }
    }
    return true;
}

// - - -

void KAsteroidsView::processBombs()
{    
    QList<Bomb*>::Iterator it;
    for(it = m_bombs.begin(); it != m_bombs.end(); it++) {
        if((*it)->process()) {
            m_player->bomb_exploded();
            Bomb* temp = (*it);                       
            m_bombs.swap(0, m_bombs.indexOf(*it));
            m_bombs.removeAt(0);

            m_explosions.push_back(new Explosion(new AnimatedPixmapItem( animation[B_EXPLOSION], &field ),
                                       temp->getX(),
                                       temp->getY(),
                                       m_explosions,
                                       FPS/4, 0, 0, 4, TILE_SIZE,
                                       &m_level_data,
                                       LEVEL_TILES_X,
                                       &m_bombs));
            delete temp;
        }
    }    

    QList<Explosion*>::Iterator explosion_it;
    for(explosion_it = m_explosions.begin(); explosion_it != m_explosions.end(); explosion_it++) {
        if((*explosion_it)->process()) {
            Explosion* temp = (*explosion_it);
            m_explosions.removeAt(m_explosions.indexOf((*explosion_it)));
            delete temp;
        }
    }

}

void KAsteroidsView::processChar()
{
    double t_speed = MOVEMENT_SPEED;

    if( (mGoUp&&mGoLeft) || (mGoUp&&mGoRight) || (mGoDown&&mGoLeft) || (mGoDown&&mGoRight)) {
        t_speed = sqrt((t_speed*t_speed)/2);
    }

    if(mGoUp) {
        /*
        if( m_level_data[ floor( (ship->x() + t_speed) /TILE_SIZE) + LEVEL_TILES_X * floor((ship->y() - t_speed)/TILE_SIZE) ] == 0
                && m_level_data[ floor( (ship->x() + ship->boundingRect().width() - t_speed )/TILE_SIZE) + LEVEL_TILES_X * floor((ship->y() - t_speed)/TILE_SIZE) ] == 0) {
            ship->setPos(ship->x(), ship->y()-t_speed );
        }

        if( m_level_data[ m_player->getUpperLeftQuadrant(TILE_SIZE, t_speed, LEVEL_TILES_X) ] == 0
                && m_level_data[ m_player->getUpperRightQuadrant(TILE_SIZE, t_speed, LEVEL_TILES_X) ] == 0) {
            m_player->setXY(m_player->getX(), m_player->getY() - t_speed);
        }
        */
        if( m_level_data[ floor( (m_player->getX() + t_speed) / TILE_SIZE) + LEVEL_TILES_X * floor((m_player->getY() - t_speed)/TILE_SIZE) ] == 0
                && m_level_data[ floor( (m_player->getX() + ship->boundingRect().width() - t_speed )/TILE_SIZE) + LEVEL_TILES_X * floor((m_player->getY() - t_speed)/TILE_SIZE) ] == 0) {
            m_player->setXY(m_player->getX(), m_player->getY() - t_speed);
        } else {
            if(!mGoLeft && !mGoRight) {
                t_speed = sqrt((t_speed*t_speed)/2);
                if(m_level_data[ floor( (m_player->getX() + t_speed) /TILE_SIZE) + LEVEL_TILES_X * floor((m_player->getY() - t_speed)/TILE_SIZE) ] != 0
                        && m_level_data[ floor( (ship->x() + m_player->getCharacterSprite()->boundingRect().width() - t_speed )/TILE_SIZE) + LEVEL_TILES_X * floor((m_player->getY() - t_speed)/TILE_SIZE) ] != 0) {} else {
                    if(m_level_data[ floor( (m_player->getX() + t_speed) /TILE_SIZE) + LEVEL_TILES_X * floor((m_player->getY() - t_speed)/TILE_SIZE) ] != 0) {
                        m_player->setXY(m_player->getX() + t_speed, m_player->getY());
                    }
                    if(m_level_data[ floor( (m_player->getX() + m_player->getCharacterSprite()->boundingRect().width() - t_speed )/TILE_SIZE) + LEVEL_TILES_X * floor((m_player->getY() - t_speed)/TILE_SIZE) ] != 0) {
                        m_player->setXY(m_player->getX() - t_speed, m_player->getY());
                    }
                }
            }
        }
/*
        else {
            if(!mGoLeft && !mGoRight) {
                t_speed = sqrt((t_speed*t_speed)/2);
                if(m_level_data[ floor( (ship->x() + t_speed) /TILE_SIZE) + LEVEL_TILES_X * floor((ship->y() - t_speed)/TILE_SIZE) ] != 0) {
                    ship->setPos(ship->x() + t_speed, ship->y());
                }
                if(m_level_data[ floor( (ship->x() + ship->boundingRect().width() - t_speed )/TILE_SIZE) + LEVEL_TILES_X * floor((ship->y() - t_speed)/TILE_SIZE) ] != 0) {
                    ship->setPos(ship->x() - t_speed, ship->y());
                }
            }
        }

        else {
            if(!mGoLeft && !mGoRight) {
                t_speed = sqrt((t_speed*t_speed)/2);
                if(m_level_data[ m_player->getUpperLeftQuadrant(TILE_SIZE, t_speed, LEVEL_TILES_X) ] != 0
                        && m_level_data[ m_player->getUpperRightQuadrant(TILE_SIZE, t_speed, LEVEL_TILES_X) ] != 0) {} else {
                    if(m_level_data[ m_player->getUpperLeftQuadrant(TILE_SIZE, t_speed, LEVEL_TILES_X) ] != 0) {
                        m_player->setXY(m_player->getX() + t_speed, m_player->getY());
                    }
                    if(m_level_data[ m_player->getUpperRightQuadrant(TILE_SIZE, t_speed, LEVEL_TILES_X) ] != 0) {
                        m_player->setXY(m_player->getX() - t_speed, m_player->getY());
                    }
                }
            }
        }
        */
    }

    if(mGoDown) {
        /*
        if( m_level_data[ floor( (ship->x() + t_speed) /TILE_SIZE) + LEVEL_TILES_X * floor((ship->y() + ship->boundingRect().height() + t_speed)/TILE_SIZE) ] == 0
                && m_level_data[ floor((ship->x() + ship->boundingRect().width() - t_speed )/TILE_SIZE) + LEVEL_TILES_X * floor((ship->y() + ship->boundingRect().height() + t_speed)/TILE_SIZE) ] == 0) {
            ship->setPos(ship->x(), ship->y()+t_speed);
        }

        if( m_level_data[ m_player->getLowerLeftQuadrant(TILE_SIZE, t_speed, LEVEL_TILES_X) ] == 0
                && m_level_data[ m_player->getLowerRightQuadrant(TILE_SIZE, t_speed, LEVEL_TILES_X) ] == 0) {
            m_player->setXY(m_player->getX(), m_player->getY() + t_speed);
        }*/
        if( m_level_data[ floor( (m_player->getX() + t_speed) /TILE_SIZE) + LEVEL_TILES_X * floor((m_player->getY() + m_player->getCharacterSprite()->boundingRect().height() + t_speed)/TILE_SIZE) ] == 0
                && m_level_data[ floor((m_player->getX() + m_player->getCharacterSprite()->boundingRect().width() - t_speed )/TILE_SIZE) + LEVEL_TILES_X * floor((m_player->getY() + m_player->getCharacterSprite()->boundingRect().height() + t_speed)/TILE_SIZE) ] == 0) {
            m_player->setXY(m_player->getX(), m_player->getY()+t_speed);
        } else {
            if(!mGoLeft && !mGoRight) {
                t_speed = sqrt((t_speed*t_speed)/2);
                if(m_level_data[ floor( (m_player->getX() + t_speed) /TILE_SIZE) + LEVEL_TILES_X * floor((m_player->getY() + m_player->getCharacterSprite()->boundingRect().height() + t_speed)/TILE_SIZE) ] != 0
                        && m_level_data[ floor((m_player->getX() + m_player->getCharacterSprite()->boundingRect().width() - t_speed )/TILE_SIZE) + LEVEL_TILES_X * floor((m_player->getY() + m_player->getCharacterSprite()->boundingRect().height() + t_speed)/TILE_SIZE) ] != 0) {} else {
                    if(m_level_data[ floor( (m_player->getX() + t_speed) /TILE_SIZE) + LEVEL_TILES_X * floor((m_player->getY() + m_player->getCharacterSprite()->boundingRect().height() + t_speed)/TILE_SIZE) ] != 0) {
                        m_player->setXY(m_player->getX() + t_speed, m_player->getY());
                    }
                    if(m_level_data[ floor((m_player->getX() + m_player->getCharacterSprite()->boundingRect().width() - t_speed )/TILE_SIZE) + LEVEL_TILES_X * floor((m_player->getY() + m_player->getCharacterSprite()->boundingRect().height() + t_speed)/TILE_SIZE) ] != 0) {
                        m_player->setXY(m_player->getX() - t_speed, m_player->getY());
                    }
                }
            }
        }
/*
        else {
            if(!mGoLeft && !mGoRight) {
                t_speed = sqrt((t_speed*t_speed)/2);
                if(m_level_data[ floor( (ship->x() + t_speed) /TILE_SIZE) + LEVEL_TILES_X * floor((ship->y() + ship->boundingRect().height() + t_speed)/TILE_SIZE) ] != 0) {
                    ship->setPos(ship->x() + t_speed, ship->y());
                }
                if(m_level_data[ floor((ship->x() + ship->boundingRect().width() - t_speed )/TILE_SIZE) + LEVEL_TILES_X * floor((ship->y() + ship->boundingRect().height() + t_speed)/TILE_SIZE) ] != 0) {
                    ship->setPos(ship->x() - t_speed, ship->y());
                }
            }
        }

        else {
            if(!mGoLeft && !mGoRight) {
                t_speed = sqrt((t_speed*t_speed)/2);
                if(m_level_data[ m_player->getLowerLeftQuadrant(TILE_SIZE, t_speed, LEVEL_TILES_X) ] != 0
                        && m_level_data[ m_player->getLowerRightQuadrant(TILE_SIZE, t_speed, LEVEL_TILES_X) ] != 0) {} else {
                    if(m_level_data[ m_player->getLowerLeftQuadrant(TILE_SIZE, t_speed, LEVEL_TILES_X) ] != 0) {
                        m_player->setXY(m_player->getX() + t_speed, m_player->getY());
                    }
                    if(m_level_data[ m_player->getLowerRightQuadrant(TILE_SIZE, t_speed, LEVEL_TILES_X) ] != 0) {
                        m_player->setXY(m_player->getX() - t_speed, m_player->getY());
                    }
                }
            }
        }*/
    }

    if(mGoLeft) {
        /*
        if( m_level_data[ floor( (ship->x()-t_speed)/TILE_SIZE) + LEVEL_TILES_X * floor((ship->y() + t_speed)/TILE_SIZE) ] == 0
                && m_level_data[ floor( (ship->x()-t_speed)/TILE_SIZE) + LEVEL_TILES_X * floor((ship->y() + ship->boundingRect().height() - t_speed )/TILE_SIZE) ] == 0) {
            ship->setPos(ship->x()-t_speed, ship->y());
        }

        if( m_level_data[ m_player->getUpperLeftQuadrant(TILE_SIZE, t_speed, LEVEL_TILES_X) ] == 0
                && m_level_data[ m_player->getLowerLeftQuadrant(TILE_SIZE, t_speed, LEVEL_TILES_X) ] == 0) {
            //ship->setPos(ship->x()-t_speed, ship->y());
            m_player->setXY(m_player->getX() - t_speed, m_player->getY());
        }
        */
        if( m_level_data[ floor( (m_player->getX() - t_speed)/TILE_SIZE) + LEVEL_TILES_X * floor((m_player->getY() + t_speed)/TILE_SIZE) ] == 0
                && m_level_data[ floor( (m_player->getX() - t_speed)/TILE_SIZE) + LEVEL_TILES_X * floor((m_player->getY() + m_player->getCharacterSprite()->boundingRect().height() - t_speed )/TILE_SIZE) ] == 0) {
            m_player->setXY(m_player->getX() - t_speed, m_player->getY());
        }
        else {
            if(!mGoUp && !mGoDown) {
                t_speed = sqrt((t_speed*t_speed)/2);
                if(m_level_data[ floor( (m_player->getX() - t_speed)/TILE_SIZE) + LEVEL_TILES_X * floor((m_player->getY() + t_speed)/TILE_SIZE) ] != 0
                        && m_level_data[ floor( (m_player->getX() - t_speed)/TILE_SIZE) + LEVEL_TILES_X * floor((m_player->getY() + m_player->getCharacterSprite()->boundingRect().height() - t_speed )/TILE_SIZE) ] != 0) {} else {
                    if(m_level_data[ floor( (m_player->getX() - t_speed)/TILE_SIZE) + LEVEL_TILES_X * floor((m_player->getY() + t_speed)/TILE_SIZE) ] != 0) {
                        m_player->setXY(m_player->getX(), m_player->getY() + t_speed);
                    }
                    if(m_level_data[ floor( (m_player->getX() - t_speed)/TILE_SIZE) + LEVEL_TILES_X * floor((m_player->getY() + m_player->getCharacterSprite()->boundingRect().height() - t_speed )/TILE_SIZE) ] != 0) {
                        m_player->setXY(m_player->getX(), m_player->getY() - t_speed);
                    }
                    /*
                    if(m_level_data[ floor( (ship->x()-t_speed)/TILE_SIZE) + LEVEL_TILES_X * floor((ship->y() + t_speed)/TILE_SIZE) ] != 0) {
                        ship->setPos(ship->x(), ship->y() + t_speed);
                    }
                    if(m_level_data[ floor( (ship->x()-t_speed)/TILE_SIZE) + LEVEL_TILES_X * floor((ship->y() + ship->boundingRect().height() - t_speed )/TILE_SIZE) ] != 0) {
                        ship->setPos(ship->x(), ship->y() - t_speed);
                    }*/
                }
            }
        }

    }

    if(mGoRight) {
        /*
        if( m_level_data[ floor( (ship->x() + ship->boundingRect().width() + t_speed)/TILE_SIZE) + LEVEL_TILES_X * floor((ship->y() + t_speed)/TILE_SIZE) ] == 0
                && m_level_data[ floor( (ship->x() + ship->boundingRect().width() + t_speed)/TILE_SIZE) + LEVEL_TILES_X * floor((ship->y() + ship->boundingRect().height() - t_speed)/TILE_SIZE) ] == 0) {
            ship->setPos(ship->x()+t_speed, ship->y());
        }

        if( m_level_data[ m_player->getUpperRightQuadrant(TILE_SIZE, t_speed, LEVEL_TILES_X) ] == 0
                && m_level_data[ m_player->getLowerRightQuadrant(TILE_SIZE, t_speed, LEVEL_TILES_X) ] == 0) {
            //ship->setPos(ship->x()+t_speed, ship->y());
            m_player->setXY(m_player->getX() + t_speed, m_player->getY());
        }
*/
        if( m_level_data[ floor( (m_player->getX() + m_player->getCharacterSprite()->boundingRect().width() + t_speed)/TILE_SIZE) + LEVEL_TILES_X * floor((m_player->getY() + t_speed)/TILE_SIZE) ] == 0
                && m_level_data[ floor( (m_player->getX() + m_player->getCharacterSprite()->boundingRect().width() + t_speed)/TILE_SIZE) + LEVEL_TILES_X * floor((m_player->getY() + m_player->getCharacterSprite()->boundingRect().height() - t_speed)/TILE_SIZE) ] == 0) {
            m_player->setXY(m_player->getX() + t_speed, m_player->getY());
        }
        else {
            if(!mGoUp && !mGoDown) {
                t_speed = sqrt((t_speed*t_speed)/2);
                if(m_level_data[ floor( (m_player->getX() + m_player->getCharacterSprite()->boundingRect().width() + t_speed)/TILE_SIZE) + LEVEL_TILES_X * floor((m_player->getY() + t_speed)/TILE_SIZE) ] != 0
                        && m_level_data[ floor( (m_player->getX() + m_player->getCharacterSprite()->boundingRect().width() + t_speed)/TILE_SIZE) + LEVEL_TILES_X * floor((m_player->getY() + m_player->getCharacterSprite()->boundingRect().height() - t_speed)/TILE_SIZE) ] != 0) {} else {
                    if(m_level_data[ floor( (m_player->getX() + m_player->getCharacterSprite()->boundingRect().width() + t_speed)/TILE_SIZE) + LEVEL_TILES_X * floor((m_player->getY() + t_speed)/TILE_SIZE) ] != 0) {
                        m_player->setXY(m_player->getX(), m_player->getY() + t_speed);
                    }
                    if(m_level_data[ floor( (m_player->getX() + m_player->getCharacterSprite()->boundingRect().width() + t_speed)/TILE_SIZE) + LEVEL_TILES_X * floor((m_player->getY() + m_player->getCharacterSprite()->boundingRect().height() - t_speed)/TILE_SIZE) ] != 0) {
                        m_player->setXY(m_player->getX(), m_player->getY() - t_speed);
                    }
                }
            }
        }

    }

    if(bombCooldown > 0) {
        bombCooldown--;
    }

    if(bombingEnabled && m_player->can_laid_bomb() && bombCooldown == 0) {

        int bomb_pos_x = (int)floor( (m_player->getX() + m_player->getCharacterSprite()->boundingRect().width()/2) / TILE_SIZE)*TILE_SIZE;
        int bomb_pos_y = (int)floor( (m_player->getY() + m_player->getCharacterSprite()->boundingRect().height()/2) / TILE_SIZE)*TILE_SIZE;

        if(check_bomb_placement_possibility(bomb_pos_x, bomb_pos_y)) {
            if(!m_bombs.isEmpty() && !(m_bombs.last()->getX() == bomb_pos_x && m_bombs.last()->getY() == bomb_pos_y)) {
                m_bombs.push_back(new Bomb(new AnimatedPixmapItem( animation[B_BOMB], &field ),
                                           bomb_pos_x,
                                           bomb_pos_y,
                                           FPS*2));
            } else if(m_bombs.isEmpty()) {
                m_bombs.push_back(new Bomb(new AnimatedPixmapItem( animation[B_BOMB], &field ),
                                           bomb_pos_x,
                                           bomb_pos_y,
                                           FPS*2));
            }

            m_player->bomb_laid();
        }

    }

    if ( ship->isVisible() ) {
        //view.centerOn(ship);
        view.centerOn(m_player->getCharacterSprite());
    }

}

double KAsteroidsView::randDouble()
{
    int v = qrand();
    return (double)v / (double)RAND_MAX;
}

int KAsteroidsView::randInt( int range )
{
    return qrand() % range;
}

void KAsteroidsView::showEvent( QShowEvent *e )
{
#if defined( QT_LICENSE_PROFESSIONAL )
    static bool wasThere = FALSE;

    if ( !wasThere ) {
        wasThere = TRUE;
        QMessageBox::information( this, tr("QGraphicsView demo"),
                                        tr("This game has been implemented using the QGraphicsView class.\n"
                                           "The QGraphicsView class is not part of the Light Platform Edition. Please \n"
                                           "contact Nokia if you want to upgrade to the Full Platform Edition.") );
    }
#endif

    QWidget::showEvent( e );
}
