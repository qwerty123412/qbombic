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
#include "gameuiconstants.h"

#define IMG_BACKGROUND GameUIConstants::getBaseDir() + "/sprites/grass_smaller.png"

#define FPS           60
#define REFRESH_DELAY           1000/FPS

#define TEXT_SPEED              4

#define MOVEMENT_SPEED          8
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
    //{ B_WALL, "wall_sprite_smaller.png", 1 },
    { B_WALL, "wall%1.png", 1 },
    { B_BOMB, "bomb_smaller%1.png", 4 },
    { B_EXPLOSION, "explosion_smaller%1.png", 2 },
    { B_PINGU, "pingu.png", 1 },
    { B_CRATE, "crate%1.png", 1 },
    { 0,                   0,                          0 }
};

KAsteroidsView::KAsteroidsView( QWidget *parent)
    : QWidget( parent),      
      field(0, 0, LEVEL_TILES_X*TILE_SIZE, LEVEL_TILES_Y*TILE_SIZE),
      view(&field, this)
{

    m_level_data = std::vector<int>(LEVEL_TILES_X*LEVEL_TILES_Y, 0); // pocet prvku, hodnota
    /*
    for(int i = 0; i < LEVEL_TILES_X; i++) {
        m_level_data[i] = B_WALL;
        m_level_data[LEVEL_TILES_X*LEVEL_TILES_Y - LEVEL_TILES_X + i] = B_WALL;
    }
    for(int i = 0; i < LEVEL_TILES_Y; i++) {
        m_level_data[LEVEL_TILES_X*i] = B_WALL;
        m_level_data[LEVEL_TILES_X*i + LEVEL_TILES_X - 1] = B_WALL;
    }
    */
    /*
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
    */

    mGoUp = false;
    mGoDown = false;
    mGoLeft = false;
    mGoRight = false;

    view.setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    view.setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );    
    view.setCacheMode(QGraphicsView::CacheBackground);
    //view.setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    view.setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
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

    bombingCooldown = 0;
    upCool = 0;
    downCool = 0;
    leftCool = 0;
    rightCool = 0;

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
    /*
    qDeleteAll(m_blocks);
    m_blocks.clear();
    */
    /*
    qDeleteAll(rocks);     rocks.clear();
    qDeleteAll(missiles);  missiles.clear();
    qDeleteAll(bits);      bits.clear();
    qDeleteAll(powerups);  powerups.clear();
    qDeleteAll(exhaust);   exhaust.clear();
    */
}

void KAsteroidsView::reset()
{
    if ( !initialized )
	return;    
    /*
    qDeleteAll(rocks);      rocks.clear();
    qDeleteAll(missiles);   missiles.clear();
    qDeleteAll(bits);       bits.clear();
    qDeleteAll(powerups);   powerups.clear();
    qDeleteAll(exhaust);    exhaust.clear();
    */
    mFrameNum = 0;
    mPaused = FALSE;

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
    /*
    qDeleteAll(rocks);     rocks.clear();
    qDeleteAll(missiles);  missiles.clear();
    qDeleteAll(bits);      bits.clear();
    qDeleteAll(powerups);  powerups.clear();
    qDeleteAll(exhaust);   exhaust.clear();
    */
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

void KAsteroidsView::updatePlayer(QString name, int x, int y, int kills, int deaths) {
    if(!players.contains(name)) {
        Character* temp = new Character(new AnimatedPixmapItem( animation[B_PINGU], &field ), x*TILE_SIZE, y*TILE_SIZE);
        players.insert(name, temp);
    } else {        
        players[name]->setTargetXY(x*TILE_SIZE, y*TILE_SIZE)->setKills(kills)->setDeaths(deaths);
        if(!m_player_name.isNull() && name == m_player_name) {
            emit killCount(players[m_player_name]->getKills());
            emit deathCount(players[m_player_name]->getDeaths());
        }
    }
}

void KAsteroidsView::newPlayer()
{
    if ( !initialized )
    return;

    mGoUp = false;
    mGoDown = false;
    mGoLeft = false;
    mGoRight = false;    
    bombingEnabled = FALSE;   
}

bool KAsteroidsView::readSprites()
{
    QString sprites_prefix = GameUIConstants::getBaseDir() + "/sprites/";

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
/*
    for(unsigned i = 0; i < m_level_data.size(); i++) {
        if(m_level_data[i] != 0) {
            AnimatedPixmapItem* test = new AnimatedPixmapItem( animation[m_level_data[i]], &field );
            test->setPos((i%LEVEL_TILES_X)*TILE_SIZE, floor(i/LEVEL_TILES_X)*TILE_SIZE);
        }
    }
*/
    /*
    m_player = new Character(new AnimatedPixmapItem( animation[B_PINGU], &field ), 2*TILE_SIZE, 2*TILE_SIZE);
    m_players.push_back(m_player);
*/
    //--T-O-D-O--: do it otherway
    //m_players.push_back(new Character(new AnimatedPixmapItem( animation[B_PINGU], &field ), 2*TILE_SIZE, 2*TILE_SIZE, this));

    //UPDATE: now this can done using create_character method that returns Character*
    //        you can do things such us following
    //qDebug() << create_character()->set_id(3)->setXY(200, 200)->get_quadrant_character_center(TILE_SIZE, LEVEL_TILES_X);

    //create_powerup();
    /*
    for(int i = 0; i < 100; i++)
        create_block();
    */

    //return (!m_player->getCharacterSprite()->image(0).isNull());
    return true;
}

void KAsteroidsView::showText( const QString &text, const QColor &color, bool scroll )
{
    /*
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
    */
}

Character* KAsteroidsView::create_character() {
    Character* temp = new Character(new AnimatedPixmapItem( animation[B_PINGU], &field ), 2*TILE_SIZE, 2*TILE_SIZE, this);
    m_players.push_back(temp);
    return temp;
}

Powerup* KAsteroidsView::create_powerup() {
    Powerup* temp = new Powerup(new AnimatedPixmapItem( animation[B_BOMB], &field ), this);
    for(;;) {
        int rand_quadrant = (qrand() % LEVEL_TILES_Y) * LEVEL_TILES_X + qrand() % LEVEL_TILES_X;
        if( m_level_data[ rand_quadrant ] == 0) {
            temp->setXY( (rand_quadrant%LEVEL_TILES_X)*TILE_SIZE, floor(rand_quadrant/LEVEL_TILES_X)*TILE_SIZE);
            m_level_data[ rand_quadrant ] = 2;
            m_powerups.push_back(temp);
            break;
        }
    }
    return temp;
}

Block* KAsteroidsView::create_block() {    
    Block* temp = new Block(new AnimatedPixmapItem( animation[B_CRATE], &field ), this);
    for(;;) {
        int rand_quadrant = (qrand() % LEVEL_TILES_Y) * LEVEL_TILES_X + qrand() % LEVEL_TILES_X;
        if( m_level_data[ rand_quadrant ] == 0) {
            temp->setXY( (rand_quadrant%LEVEL_TILES_X)*TILE_SIZE, floor(rand_quadrant/LEVEL_TILES_X)*TILE_SIZE);
            m_level_data[ rand_quadrant ] = 11;
            m_blocks.push_back(temp);
            break;
        }
    }
    return temp;
}

void KAsteroidsView::hideText()
{
    textDy = -TEXT_SPEED;
}

void KAsteroidsView::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    field.setSceneRect(0, 0, width()-4, height()-4);
    view.resize(width(),height());
}

void KAsteroidsView::timerEvent( QTimerEvent * )
{
    field.advance();

    processChar();    

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

    if(upCool > 0)
        upCool--;

    if(downCool > 0)
        downCool--;

    if(leftCool > 0)
        leftCool--;

    if(rightCool > 0)
        rightCool--;

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

void KAsteroidsView::processBombs()
{
    QList<Bomb*>::Iterator it;
    for(it = m_bombs.begin(); it != m_bombs.end(); it++) {
        if((*it)->process()) {
            m_player->bomb_exploded();
            Bomb* temp = (*it);                       
            m_bombs.swap(0, m_bombs.indexOf(*it));
            m_bombs.removeAt(0);

            m_level_data[ (temp->getY()/TILE_SIZE) * LEVEL_TILES_X + (temp->getX() / TILE_SIZE) ] = 0;

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
            m_level_data[ (temp->getY()/TILE_SIZE) * LEVEL_TILES_X + (temp->getX() / TILE_SIZE) ] = 0;
            m_explosions.removeAt(m_explosions.indexOf((*explosion_it)));
            delete temp;
        }
    }

}

void KAsteroidsView::setPlayerCoordsXY(int x, int y) {
    m_player->setXY(x*TILE_SIZE, y*TILE_SIZE);
    //m_player->setTargetXY(x*TILE_SIZE, y*TILE_SIZE);
    //qDebug() << "player position " << x << " " << y;
}

void KAsteroidsView::addUndestroyable(int x, int y) {
    if(m_level_data[LEVEL_TILES_X * y + x] == 0) {
        AnimatedPixmapItem* temp = new AnimatedPixmapItem( animation[B_WALL], &field );
        temp->setPos(x*TILE_SIZE, y*TILE_SIZE);
        m_level_data[ LEVEL_TILES_X * y + x ] = 1;
    }
}

void KAsteroidsView::addWall(int x, int y) {
    if(m_level_data[LEVEL_TILES_X * y + x] == 0) {
        Block* temp = new Block(new AnimatedPixmapItem( animation[B_CRATE], &field ), this);
        temp->setXY( x*TILE_SIZE, y*TILE_SIZE );
        temp->activate();
        m_level_data[ LEVEL_TILES_X * y + x ] = 11;
        m_blocks.push_back(temp);        
    } else {
        for(Block* block : m_blocks) {
            if(block->get_quadrant(TILE_SIZE, LEVEL_TILES_X) == LEVEL_TILES_X * y + x) {
                block->activate();
                break;
            }
        }
    }
}

void KAsteroidsView::addBomb(int x, int y) {
    if(m_level_data[LEVEL_TILES_X * y + x] == 0) {
        Bomb* temp = new Bomb(new AnimatedPixmapItem( animation[B_BOMB], &field ), 0, 0, 0, this);
        temp->setXY( x*TILE_SIZE, y*TILE_SIZE );
        temp->activate();
        m_level_data[ LEVEL_TILES_X * y + x ] = 10;
        m_bombs.push_back(temp);
    } else {
        for(Bomb* bomb : m_bombs) {
            if(bomb->get_quadrant(TILE_SIZE, LEVEL_TILES_X) == LEVEL_TILES_X * y + x) {
                bomb->activate();
                break;
            }
        }
    }
}

void KAsteroidsView::addExplosion(int x, int y) {
    if(m_level_data[LEVEL_TILES_X * y + x] == 0) {
        Explosion* temp = new Explosion(new AnimatedPixmapItem( animation[B_EXPLOSION], &field ), 0, 0);
        temp->setXY( x*TILE_SIZE, y*TILE_SIZE );
        temp->activate();
        m_level_data[ LEVEL_TILES_X * y + x ] = 9;
        m_explosions.push_back(temp);
    } else {
        for(Explosion* explosion : m_explosions) {
            if(explosion->get_quadrant(TILE_SIZE, LEVEL_TILES_X) == LEVEL_TILES_X * y + x) {
                explosion->activate();
                break;
            }
        }
    }
}

void KAsteroidsView::addPowerup(int x, int y) {
    if(m_level_data[LEVEL_TILES_X * y + x] == 0) {
        Powerup* temp = new Powerup(new AnimatedPixmapItem( animation[B_EXPLOSION], &field ), this);
        temp->setXY( x*TILE_SIZE, y*TILE_SIZE );
        temp->activate();
        m_level_data[ LEVEL_TILES_X * y + x ] = 8;
        m_powerups.push_back(temp);
    } else {
        for(Powerup* powerup : m_powerups) {
            if(powerup->get_quadrant(TILE_SIZE, LEVEL_TILES_X) == LEVEL_TILES_X * y + x) {
                powerup->activate();
                break;
            }
        }
    }
}

void KAsteroidsView::blocks_deactivate() {
    for(Block* block : m_blocks) {
        block->deactivate();
    }
}

void KAsteroidsView::delete_deactivated_blocks() {
    for(Block* block : m_blocks) {
        if(!block->is_activated()) {
            m_level_data[ block->get_quadrant(TILE_SIZE, LEVEL_TILES_X) ] = 0;
            m_blocks.swap(0, m_blocks.indexOf(block));
            m_blocks.removeAt(0);
            block->delete_sprite();
            delete block;
        }
    }
}

void KAsteroidsView::bombs_deactivate() {
    for(Bomb* bomb : m_bombs) {
        bomb->deactivate();
    }
}

void KAsteroidsView::delete_deactivated_bombs() {
    for(Bomb* bomb : m_bombs) {
        if(!bomb->is_activated()) {
            m_level_data[ bomb->get_quadrant(TILE_SIZE, LEVEL_TILES_X) ] = 0;
            m_bombs.swap(0, m_bombs.indexOf(bomb));
            m_bombs.removeAt(0);
            delete bomb;
        }
    }
}

void KAsteroidsView::explosions_deactivate() {
    for(Explosion* explosion : m_explosions) {
        explosion->deactivate();
    }
}

void KAsteroidsView::delete_deactivated_explosions() {
    for(Explosion* explosion : m_explosions) {
        if(!explosion->is_activated()) {
            m_level_data[ explosion->get_quadrant(TILE_SIZE, LEVEL_TILES_X) ] = 0;
            //explosion->getBomb()->setFrame(1);
            m_explosions.swap(0, m_explosions.indexOf(explosion));
            m_explosions.removeAt(0);
            delete explosion;
        }
    }
}

void KAsteroidsView::powerups_deactivate() {
    for(Powerup* powerup : m_powerups) {
        powerup->deactivate();
    }
}

void KAsteroidsView::delete_deactivated_powerups() {
    for(Powerup* powerup : m_powerups) {
        if(!powerup->is_activated()) {
            m_level_data[ powerup->get_quadrant(TILE_SIZE, LEVEL_TILES_X) ] = 0;
            m_powerups.swap(0, m_powerups.indexOf(powerup));
            m_powerups.removeAt(0);
            powerup->delete_sprite();
            delete powerup;
        }
    }
}

void KAsteroidsView::processChar()
{

    QMapIterator<QString, Character*> i(players);
    while (i.hasNext()) {
        i.next();        

        Character* ch = i.value();

        if( abs(ch->getTargetX() - ch->getX()) < MOVEMENT_SPEED ) {
            ch->setXY(ch->getTargetX(), ch->getY());
        } else {
            if(ch->getTargetX() - ch->getX() > 0) {
                ch->setXY(ch->getX() + MOVEMENT_SPEED, ch->getY());
            } else {
                ch->setXY(ch->getX() - MOVEMENT_SPEED, ch->getY());
            }
        }

        if( abs(ch->getTargetY() - ch->getY()) < MOVEMENT_SPEED ) {
            ch->setXY(ch->getX(), ch->getTargetY());
        } else {
            if(ch->getTargetY() - ch->getY() > 0) {
                ch->setXY(ch->getX(), ch->getY() + MOVEMENT_SPEED);
            } else {
                ch->setXY(ch->getX(), ch->getY() - MOVEMENT_SPEED);
            }
        }

    }

    if(!m_player_name.isNull()) {
        view.centerOn(players[m_player_name]->getCharacterSprite());
    }

    int cooldown = 5;

    if(mGoUp && !upCool) {
        upCool = cooldown;
        emit move(0);
    }

    if(mGoDown && !downCool) {
        downCool = cooldown;
        emit move(2);
    }

    if(mGoLeft && !leftCool) {
        leftCool = cooldown;
        emit move(3);
    }

    if(mGoRight && !rightCool) {
        rightCool = cooldown;
        emit move(1);
    }

    if(bombingEnabled) {
        emit bombLaid();
    }

}

void KAsteroidsView::center_on(QString name) {
    if(m_player_name.isNull()) {
        m_player_name = name;
    }
}

void KAsteroidsView::processDeaths() {
    for (Character* player : m_players)
    {
        if (m_level_data[player->get_quadrant_character_center(TILE_SIZE, LEVEL_TILES_X)] == 1)
            player->setXY(TILE_SIZE, TILE_SIZE);
    }
}

void KAsteroidsView::processPowerups() {
    for (Character* player : m_players)
    {
        if (m_level_data[player->get_quadrant_character_center(TILE_SIZE, LEVEL_TILES_X)] == 2)
        {
            player->raise_bomb_limit();
            m_level_data[player->get_quadrant_character_center(TILE_SIZE, LEVEL_TILES_X)] = 0;

            for (Powerup* powUp : m_powerups)
            {
                if (powUp->get_quadrant(TILE_SIZE, LEVEL_TILES_X) == player->get_quadrant_character_center(TILE_SIZE, LEVEL_TILES_X))
                {
                    m_powerups.swap(0, m_powerups.indexOf(powUp));
                    m_powerups.removeAt(0);
                    delete powUp;
                }
            }
        }
    }
}

void KAsteroidsView::processBlocks() {

    for(Block* block : m_blocks)
    {
        if (m_level_data[block->get_quadrant(TILE_SIZE, LEVEL_TILES_X)] == 0)
        {
            m_blocks.swap(0, m_blocks.indexOf(block));
            m_blocks.removeAt(0);
            delete block;
        }
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
