/*******************************************************************
 *
 * Copyright 2007 Dmitry Suzdalev <dimsuz@gmail.com>
 *
 * This file is part of the KDE project "KAtomic"
 *
 * KAtomic is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * KAtomic is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with KAtomic; see the file COPYING.  If not, write to
 * the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 ********************************************************************/
#include "gamemessage.h"
#include <QPainter>
#include <QTimeLine>
#include <QTimer>
#include <QGraphicsScene>

#include <KIcon>
#include <kdebug.h>

// margin on the sides of message box
static const int MARGIN = 15;
// offset of message from start of the scene
static const int SHOW_OFFSET = 5;
// size of icon pixmap
static const int PIX_SIZE = 32;
// space between pixmap and text
static const int SOME_SPACE = 10;

class GameMessageItemPrivate
{
public:
    GameMessageItemPrivate() : m_position( GameMessageItem::BottomLeft ), m_timeout(2000) {}
    /**
     * Text to show
     */
    QString m_text;
    /**
     * Timeline for animations
     */
    QTimeLine m_timeLine;
    /**
     * Timer used to start hiding
     */
    QTimer m_timer;
    /**
     * Holds bounding rect of an item
     */
    QRectF m_boundRect;
    /**
     * Position where item will appear
     */
    GameMessageItem::Position m_position;
    /**
     * Timeout to stay visible on screen
     */
    int m_timeout;
    /**
     * Pixmap to display at the left of the text
     */
    QPixmap m_iconPix;
};

GameMessageItem::GameMessageItem()
    : d(new GameMessageItemPrivate)
{
    hide();

    KIcon infoIcon("dialog-information");
    d->m_iconPix = infoIcon.pixmap(PIX_SIZE, PIX_SIZE);
    d->m_timer.setSingleShot(true);

    connect( &d->m_timeLine, SIGNAL(frameChanged(int)), SLOT(animationFrame(int)) );
    connect( &d->m_timeLine, SIGNAL(finished()), SLOT(hideMe()));
    connect( &d->m_timer, SIGNAL(timeout()), SLOT(timeout()) );
}

void GameMessageItem::paint( QPainter* p, const QStyleOptionGraphicsItem *option, QWidget* widget )
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if( d->m_text.isEmpty() )
        return;

    p->setBrush( Qt::lightGray );
    p->setOpacity(0.9);
    p->drawRect( d->m_boundRect );
    p->drawPixmap( MARGIN, static_cast<int>(d->m_boundRect.height()/2) - d->m_iconPix.height()/2,
                   d->m_iconPix );
    p->drawText( d->m_boundRect.adjusted( MARGIN+PIX_SIZE+SOME_SPACE, MARGIN, -MARGIN, -MARGIN ),
                 d->m_text );
}

void GameMessageItem::showMessage( const QString& text, Position pos )
{
    if(d->m_timeLine.state() == QTimeLine::Running || d->m_timer.isActive())
        return;// we're already showing a message

    d->m_text = text;
    d->m_position = pos;
    d->m_timeLine.setDirection( QTimeLine::Forward );

    // recalculate bounding rect
    QFont f; // Taking default application font. Hope this is ok.
    QFontMetrics fm( f );
    d->m_boundRect = QRectF(0, 0,
                            fm.width( d->m_text )+MARGIN*2+PIX_SIZE+SOME_SPACE,
                            fm.height()+MARGIN*2);

    // setup animation
    d->m_timeLine.setDuration(300);
    if( d->m_position == TopLeft || d->m_position == TopRight )
        d->m_timeLine.setFrameRange( static_cast<int>(-d->m_boundRect.height()-SHOW_OFFSET),
                                   SHOW_OFFSET );
    else if( d->m_position == BottomLeft || d->m_position == BottomRight )
        d->m_timeLine.setFrameRange( static_cast<int>(scene()->height()+SHOW_OFFSET),
                                   static_cast<int>(scene()->height()-d->m_boundRect.height()-SHOW_OFFSET) );

    // move to the start position
    animationFrame(d->m_timeLine.startFrame());
    show();
    d->m_timeLine.start();
    // 300 msec to animate showing message + d->m_timeout to stay visible => then hide
    d->m_timer.start( 300+d->m_timeout );
}

void GameMessageItem::animationFrame(int frame)
{
    if( d->m_position == TopLeft || d->m_position == BottomLeft )
        setPos( SHOW_OFFSET, frame );
    else if( d->m_position == TopRight || d->m_position == BottomRight )
        setPos( scene()->width()-d->m_boundRect.width()-SHOW_OFFSET, frame );
}

void GameMessageItem::timeout()
{
    // let's hide
    d->m_timeLine.setDirection( QTimeLine::Backward );
    d->m_timeLine.start();
}

void GameMessageItem::setMessageTimeOut( int msec )
{
    d->m_timeout = msec;
}

QRectF GameMessageItem::boundingRect() const
{
    return d->m_boundRect;
}

GameMessageItem::~GameMessageItem()
{
    delete d;
}

void GameMessageItem::hideMe()
{
    // if we just got moved out of visibility, let's do more - let's hide :)
    if( d->m_timeLine.direction() == QTimeLine::Backward )
        hide();
}

#include "gamemessage.moc"
