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
//	--- toplevel.cpp ---
#include <QAction>
#include <QLabel>
#include <QLayout>
#include <QLCDNumber>
#include <QPushButton>

#include <QApplication>
#include <QHBoxLayout>
#include <QShowEvent>
#include <QFrame>
#include <QPixmap>
#include <QHideEvent>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QDir>

#include "toplevel.h"
#include "ledmeter.h"
#include "gameuiconstants.h"


#define SB_SCORE	1
#define SB_LEVEL	2
#define SB_SHIPS	3

struct SLevel
{
    int    nrocks;
    double rockSpeed;
};

#define MAX_LEVELS	16

SLevel levels[MAX_LEVELS] =
{
    { 1, 0.4 },
    { 1, 0.6 },
    { 2, 0.5 },
    { 2, 0.7 },
    { 2, 0.8 },
    { 3, 0.6 },
    { 3, 0.7 },
    { 3, 0.8 },
    { 4, 0.6 },
    { 4, 0.7 },
    { 4, 0.8 },
    { 5, 0.7 },
    { 5, 0.8 },
    { 5, 0.9 },
    { 5, 1.0 }
};

const char *soundEvents[] =
{
    "ShipDestroyed",
    "RockDestroyed",
    0
};

const char *soundDefaults[] =
{
    "Explosion.wav",
    "ploop.wav",
    0
};


KAstTopLevel::KAstTopLevel( QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *border = new QWidget( this );

    QPalette palette;
    palette.setColor(border->backgroundRole(), Qt::black);
    border->setPalette(palette);

    setCentralWidget( border );

    QVBoxLayout *borderLayout = new QVBoxLayout( border );
    borderLayout->addStretch( 1 );

    QWidget *mainWin = new QWidget( border );
    mainWin->setFixedSize(960, 640);

    borderLayout->addWidget( mainWin, 1, Qt::AlignHCenter );

    borderLayout->addStretch( 1 );

    view = new KAsteroidsView( mainWin );
    view->setFocusPolicy( Qt::StrongFocus );
    connect( view, SIGNAL(shipKilled()), SLOT(slotShipKilled()) );
    connect( view, SIGNAL(rockHit(int)), SLOT(slotRockHit(int)) );
    connect( view, SIGNAL(rocksRemoved()), SLOT(slotRocksRemoved()) );
    connect( view, SIGNAL(updateVitals()), SLOT(slotUpdateVitals()) );

    connect( view, SIGNAL(move(int)), this, SLOT(slotProcessMovements(int)));
    connect( view, SIGNAL(bombLaid()), this, SLOT(slotProcessBomb()));
    connect( view, SIGNAL(killCount(int)), this, SLOT(slotKillCount(int)) );
    connect( view, SIGNAL(deathCount(int)), this, SLOT(slotDeathCount(int)) );

    QVBoxLayout *vb = new QVBoxLayout( mainWin );
    QHBoxLayout *hb = new QHBoxLayout;
    QHBoxLayout *hbd = new QHBoxLayout;
    vb->addLayout( hb );

    QFont labelFont( "helvetica", 24 );

    QPalette pal(Qt::darkGreen, Qt::black, QColor( 128, 128, 128 ), QColor( 64, 64, 64 ), Qt::black, Qt::darkGreen, Qt::black);

    mainWin->setPalette( pal );

    hb->addSpacing( 180 );

    QLabel *label;
    label = new QLabel( tr("Kills"), mainWin );
    label->setFont( labelFont );
    label->setPalette( pal );
    label->setFixedWidth( label->sizeHint().width() );
    hb->addWidget( label );

    scoreLCD = new QLCDNumber( 6, mainWin );
    scoreLCD->setFrameStyle( QFrame::NoFrame );
    scoreLCD->setSegmentStyle( QLCDNumber::Flat );
    scoreLCD->setFixedWidth( 150 );
    scoreLCD->setPalette( pal );
    hb->addWidget( scoreLCD );
    hb->addStretch( 10 );

    label = new QLabel( tr("Deaths"), mainWin );
    label->setFont( labelFont );
    label->setPalette( pal );
    label->setFixedWidth( label->sizeHint().width() );
    hb->addWidget( label );

    levelLCD = new QLCDNumber( 6, mainWin );
    levelLCD->setFrameStyle( QFrame::NoFrame );
    levelLCD->setSegmentStyle( QLCDNumber::Flat );
    levelLCD->setFixedWidth( 150 );
    levelLCD->setPalette( pal );
    hb->addWidget( levelLCD );
    hb->addStretch( 10 );

    /*
    label = new QLabel( tr("Ships"), mainWin );
    label->setFont( labelFont );
    label->setFixedWidth( label->sizeHint().width() );
    label->setPalette( pal );
    hb->addWidget( label );

    shipsLCD = new QLCDNumber( 1, mainWin );
    shipsLCD->setFrameStyle( QFrame::NoFrame );
    shipsLCD->setSegmentStyle( QLCDNumber::Flat );
    shipsLCD->setFixedWidth( 40 );
    shipsLCD->setPalette( pal );
    hb->addWidget( shipsLCD );
*/
    hb->addStrut( 30 );

    vb->addWidget( view, 10 );

// -- bottom layout:
    vb->addLayout( hbd );

    QFont smallFont( "helvetica", 14 );
    hbd->addSpacing( 10 );

    QString sprites_prefix = GameUIConstants::getBaseDir() + "/sprites/";
/*
    QPixmap pm( sprites_prefix + "powerups/brake.png" );
    label = new QLabel( mainWin );
    label->setPixmap( pm );
    label->setFixedWidth( label->sizeHint().width() );
    label->setPalette( pal );
    hbd->addWidget( label );

    brakesLCD = new QLCDNumber( 1, mainWin );
    brakesLCD->setFrameStyle( QFrame::NoFrame );
    brakesLCD->setSegmentStyle( QLCDNumber::Flat );
    brakesLCD->setPalette( pal );
    brakesLCD->setFixedHeight( 20 );
    hbd->addWidget( brakesLCD );

    hbd->addSpacing( 10 );

    pm.load( sprites_prefix + "powerups/shield.png" );
    label = new QLabel( mainWin );
    label->setPixmap( pm );
    label->setFixedWidth( label->sizeHint().width() );
    label->setPalette( pal );
    hbd->addWidget( label );

    shieldLCD = new QLCDNumber( 1, mainWin );
    shieldLCD->setFrameStyle( QFrame::NoFrame );
    shieldLCD->setSegmentStyle( QLCDNumber::Flat );
    shieldLCD->setPalette( pal );
    shieldLCD->setFixedHeight( 20 );
    hbd->addWidget( shieldLCD );

    hbd->addSpacing( 10 );

    pm.load( sprites_prefix + "powerups/shoot.png" );
    label = new QLabel( mainWin );
    label->setPixmap( pm );
    label->setFixedWidth( label->sizeHint().width() );
    label->setPalette( pal );
    hbd->addWidget( label );

    shootLCD = new QLCDNumber( 1, mainWin );
    shootLCD->setFrameStyle( QFrame::NoFrame );
    shootLCD->setSegmentStyle( QLCDNumber::Flat );
    shootLCD->setPalette( pal );
    shootLCD->setFixedHeight( 20 );
    hbd->addWidget( shootLCD );

    hbd->addStretch( 1 );

    label = new QLabel( tr( "Fuel" ), mainWin );
    label->setFont( smallFont );
    label->setFixedWidth( label->sizeHint().width() + 10 );
    label->setPalette( pal );
    hbd->addWidget( label );

    powerMeter = new KALedMeter( mainWin );
    powerMeter->setFrameStyle( QFrame::Box | QFrame::Plain );
    powerMeter->setRange( MAX_POWER_LEVEL );
    powerMeter->addColorRange( 10, Qt::darkRed );
    powerMeter->addColorRange( 20, QColor(160, 96, 0) );
    powerMeter->addColorRange( 70, Qt::darkGreen );
    powerMeter->setCount( 40 );
    powerMeter->setPalette( pal );
    powerMeter->setFixedSize( 200, 12 );
    hbd->addWidget( powerMeter );
*/
    shipsRemain = 3;
    showHiscores = FALSE;

    actions.insert( Qt::Key_Space, Bomb );
    actions.insert( Qt::Key_N, NewGame );

    actions.insert( Qt::Key_W, CharUp );
    actions.insert( Qt::Key_A, CharLeft );
    actions.insert( Qt::Key_S, CharDown );
    actions.insert( Qt::Key_D, CharRight );

    //view->showText( tr( "Press N to start playing" ), Qt::yellow );
}

KAstTopLevel::~KAstTopLevel()
{
}

void KAstTopLevel::playSound( const char * )
{
}

void KAstTopLevel::slotProcessMovements(int direction) {
    emit move(direction);
}

void KAstTopLevel::slotProcessBomb() {
    emit bombLaid();
}

void KAstTopLevel::slotKillCount(int kills) {
    scoreLCD->display( kills );
}

void KAstTopLevel::slotDeathCount(int deaths) {
    levelLCD->display( deaths );
}

void KAstTopLevel::keyPressEvent( QKeyEvent *event )
{
    if ( event->isAutoRepeat() || !actions.contains( event->key() ) )
    {
        event->ignore();
        return;
    }

    Action a = actions[ event->key() ];

    switch ( a )
    {
        case CharUp:
            view->goUp( TRUE );
            break;

        case CharLeft:
            view->goLeft( TRUE );
            break;

        case CharDown:
            view->goDown( TRUE );
            break;

        case CharRight:
            view->goRight( TRUE );
            break;

        case Bomb:
            view->bomb( TRUE );
            break;

        default:
            event->ignore();
            return;
    }
    event->accept();
}

void KAstTopLevel::keyReleaseEvent( QKeyEvent *event )
{
    if ( event->isAutoRepeat() || !actions.contains( event->key() ) )
    {
        event->ignore();
        return;
    }

    Action a = actions[ event->key() ];

    switch ( a )
    {
        case CharUp:
            view->goUp( FALSE );
            break;

        case CharLeft:
            view->goLeft( FALSE );
            break;

        case CharDown:
            view->goDown( FALSE );
            break;

        case CharRight:
            view->goRight( FALSE );
            break;

        case Bomb:
            view->bomb( FALSE );
            break;

        case Launch:
            if ( waitShip )
            {
                //view->newShip();
                view->newPlayer();
                waitShip = FALSE;
                view->hideText();
            }
            else
            {
                event->ignore();
                return;
            }
            break;

        case NewGame:
            slotNewGame();
            break;

        default:
            event->ignore();
            return;
    }

    event->accept();
}

void KAstTopLevel::showEvent( QShowEvent *e )
{
    QMainWindow::showEvent( e );
    view->pause( FALSE );
    view->setFocus();
}

void KAstTopLevel::hideEvent( QHideEvent *e )
{
    QMainWindow::hideEvent( e );
    view->pause( TRUE );
}

void KAstTopLevel::slotNewGame()
{

    score = 0;
    shipsRemain = SB_SHIPS;
    //scoreLCD->display( 0 );
    level = 0;
    //levelLCD->display( 0 );
    //shipsLCD->display( shipsRemain-1 );

    view->newGame();
    //view->newShip();
    view->newPlayer();

    waitShip = FALSE;
    view->hideText();
    isPaused = FALSE;
}

void KAstTopLevel::slotShipKilled()
{
    shipsRemain--;
    //shipsLCD->display( shipsRemain-1 );

    playSound( "ShipDestroyed" );

    if ( shipsRemain )
    {
        waitShip = TRUE;
        //view->showText( tr( "Ship Destroyed. Press L to launch."), Qt::yellow );
    }
    else
    {
        //view->showText( tr("Game Over!"), Qt::red );
        view->endGame();
	doStats();
//        highscore->addEntry( score, level, showHiscores );
    }
}

void KAstTopLevel::slotRockHit( int size )
{
    switch ( size )
    {
	case 0:
	    score += 10;
	     break;

	case 1:
	    score += 20;
	    break;

	default:
	    score += 40;
      }

    playSound( "RockDestroyed" );

    scoreLCD->display( score );
}

void KAstTopLevel::slotRocksRemoved()
{
    level++;

    if ( level >= MAX_LEVELS )
	level = MAX_LEVELS - 1;

    //view->setRockSpeed( levels[level-1].rockSpeed );
    //view->addRocks( levels[level-1].nrocks );

    levelLCD->display( level+1 );
}

void KAstTopLevel::doStats()
{
    QString r( "0.00" );
    /*
    if ( view->shots() )
	 r = QString::number( (double)view->hits() / view->shots() * 100.0,
			     'g', 2 );
                 */

/* multi-line text broken in Qt 3
    QString s = tr( "Game Over\n\nShots fired:\t%1\n  Hit:\t%2\n  Missed:\t%3\nHit ratio:\t%4 %\n\nPress N for a new game" )
      .arg(view->shots()).arg(view->hits())
      .arg(view->shots() - view->hits())
      .arg(r);
*/

    //view->showText( "Game Over.   Press N for a new game.", Qt::yellow, FALSE );
}

void KAstTopLevel::slotUpdateVitals()
{
    //brakesLCD->display( view->brakeCount() );
    //shieldLCD->display( view->shieldCount() );
    //shootLCD->display( view->shootCount() );
    //teleportsLCD->display( view->teleportCount() );
    //powerMeter->setValue( view->power() );
}
