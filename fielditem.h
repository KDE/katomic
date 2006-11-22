/*******************************************************************
 *
 * Copyright 2006 Dmitry Suzdalev <dimsuz@gmail.com>
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
#ifndef FIELD_ITEM_H
#define FIELD_ITEM_H

#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>

// FIXME dimsuz: document all classes here!

class FieldItem : public QGraphicsPixmapItem
{
public:
    explicit FieldItem( QGraphicsScene* scene )
        : QGraphicsPixmapItem( 0, scene ), m_fieldX(0), m_fieldY(0)
    { setShapeMode( BoundingRectShape ); }

    void setFieldX(int x) { m_fieldX = x; }
    void setFieldY(int y) { m_fieldY = y; }
    void setFieldXY(int x, int y) { m_fieldX = x; m_fieldY = y; }

    int fieldX() const { return m_fieldX; }
    int fieldY() const { return m_fieldY; }

    // enable use of qgraphicsitem_cast
    enum { Type = UserType + 1 };
    virtual int type() const { return Type; }
private:
    int m_fieldX;
    int m_fieldY;
};

class AtomFieldItem : public FieldItem
{
public:
    explicit AtomFieldItem( QGraphicsScene* scene )
        : FieldItem(scene), m_atomNum(-1) { }

    void setAtomNum(int n) { m_atomNum = n; }
    int atomNum() const { return m_atomNum; }

    // enable use of qgraphicsitem_cast
    enum { Type = UserType + 2 };
    virtual int type() const { return Type; }
private:
    // from molecule
    int m_atomNum; 
};

class QTimeLine;
class ArrowFieldItem : public QObject, public FieldItem
{
    Q_OBJECT
public:
    explicit ArrowFieldItem( QGraphicsScene* scene );
    virtual ~ArrowFieldItem();

    // enable use of qgraphicsitem_cast
    enum { Type = UserType + 3 };
    virtual int type() const { return Type; }
private slots:
    void setOpacity( qreal opacity );
private:
    void paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
    QVariant itemChange( GraphicsItemChange change, const QVariant& value );

    qreal m_opacity;
    QTimeLine *m_timeLine;
};

class MoleculeRenderer;
class Molecule;
class PlayField;

class MoleculePreviewItem : public QGraphicsItem
{
public:
    explicit MoleculePreviewItem( PlayField* scene );
    ~MoleculePreviewItem();

    void setMolecule( const Molecule* mol );

    void setWidth( int width );

    void setMaxAtomSize( int maxSize );
    inline QRectF boundingRect() const { return QRectF(0,0, m_width, m_width+m_butRect.height()+2); }

private:
    void paint( QPainter * painter, const QStyleOptionGraphicsItem*, QWidget * widget = 0 );
    void hoverMoveEvent( QGraphicsSceneHoverEvent* ev );
    void mousePressEvent( QGraphicsSceneMouseEvent* ev );

    int m_width;
    int m_maxAtomSize;
    MoleculeRenderer *m_molRenderer;

    QRect m_butRect;
    bool m_hovered;
    bool m_pressed;
};

class MoleculeInfoItem : public QGraphicsTextItem
{
public:
    explicit MoleculeInfoItem( QGraphicsScene* scene );
    void setMolecule( const Molecule& mol );
    void setSize( int width, int height );
    // used in playfield to optimize animation -> disables text rendering (only bounding rect)
    void setShowInfo(bool show) { m_showInfo = show; update(); }
    inline QRectF boundingRect() const { return QRectF(0,0, m_width, m_height); }
private:
    void paint( QPainter * painter, const QStyleOptionGraphicsItem*, QWidget * widget = 0 );

    int m_width;
    int m_height;
    bool m_showInfo;
};

#endif
