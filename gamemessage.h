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
#ifndef GAME_MESSAGE_ITEM_H
#define GAME_MESSAGE_ITEM_H

#include <QGraphicsItem>
#include <QObject>

class GameMessageItemPrivate;

/**
 * QGraphicsItem capable of showing short game messages
 * which do not interrupt the gameplay.
 * Messages can stay on screen for specified amount of time
 * and automatically hide after.
 *
 * Example of use:
 * \code
 * GameMessageItem *messageItem = new GameMessageItem();
 * myGraphicsScene->addItem(messageItem);
 * ...
 * messageItem->setMessageTimeOut( 3000 ); // 3 sec
 * messageItem->setPosition( BottomLeft );
 * messageItem->showMessage("Hello, I'm a game message! How do you do?");
 * \endcode
 */
class GameMessageItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    /**
     * The possible places in the scene where a message can be shown
     */
    enum Position { TopLeft, TopRight, BottomLeft, BottomRight };
    /**
     * Constructs a message item. It is hidden by default.
     */
    GameMessageItem();
    /**
     * Destructs a message item
     */
    ~GameMessageItem();
    /**
     * Sets the amount of time the item will stay visible on screen
     * before it goes away.
     * By default item is shown for 2000 msec
     * If item is hovered with mouse it will hide only after
     * user moves the mouse away
     *
     * @param msec amount of time in milliseconds
     */
    void setMessageTimeOut( int msec );
    /**
     * Shows the message to item: item will appear at specified place
     * of the scene using simple animation
     * Item will be automatically hidden after timeout set in setMessageTimeOut() passes
     * If item is hovered with mouse it won't hide until user moves
     * the mouse away
     *
     * @param text holds the message to show
     * @param pos position on the scene where the message will appear
     */
    void showMessage( const QString& text, Position pos );
    /**
     * @return the bounding rect of this item. Reimplemented from QGraphicsItem
     */
    virtual QRectF boundingRect() const;
    /**
     * Paints item. Reimplemented from QGraphicsItem
     */
    virtual void paint( QPainter* p, const QStyleOptionGraphicsItem *option, QWidget* widget );
private Q_SLOTS:
    void animationFrame(int);
    void hideMe();
    void playHideAnimation();
private:
    virtual void hoverEnterEvent( QGraphicsSceneHoverEvent* );
    virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent* );
    GameMessageItemPrivate * const d;
};

#endif
