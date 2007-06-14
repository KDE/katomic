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
     * @param msec amount of time in milliseconds
     */
    void setMessageTimeOut( int msec );
    /**
     * Shows the message to item: item will appear at specified place
     * of the scene using simple animation
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
private slots:
    void animationFrame(int);
    void timeout();
private:
    GameMessageItemPrivate * const d;
};

#endif
