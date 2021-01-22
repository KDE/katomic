/*
    This file is part of the KDE project "KAtomic"

    SPDX-FileCopyrightText: 2006-2007 Dmitry Suzdalev <dimsuz@gmail.com>
    SPDX-FileCopyrightText: 2010 Brian Croom <brian.s.croom@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef FIELD_ITEM_H
#define FIELD_ITEM_H

#include "playfield.h" // for enum PlayField::Direction

#include <KGameRenderedItem>

#include <QGraphicsTextItem>

class KGameRenderer;
class atom;

/**
 *  Represents item that can be placed in the PlayField.
 *  Basically it just extends QGraphicsPixmapItem by understanding
 *  field's cellbased coords.
 */
class FieldItem : public KGameRenderedItem
{
public:
    explicit FieldItem( KGameRenderer* renderer, const QString& spriteKey, QGraphicsScene* scene );

    void setFieldX(int x) { m_fieldX = x; }
    void setFieldY(int y) { m_fieldY = y; }
    void setFieldXY(int x, int y) { m_fieldX = x; m_fieldY = y; }

    int fieldX() const { return m_fieldX; }
    int fieldY() const { return m_fieldY; }

    // enable use of qgraphicsitem_cast
    enum { Type = UserType + 1 };
    int type() const override { return Type; }
private:
    int m_fieldX;
    int m_fieldY;
};

/**
 *  FieldItem that knows what atom number it holds
 *  @see Molecule
 */
class AtomFieldItem : public FieldItem
{
public:
    explicit AtomFieldItem(KGameRenderer* renderer, atom at, QGraphicsScene* scene );

    void setAtomNum(int n) { m_atomNum = n; }
    int atomNum() const { return m_atomNum; }

    /**
     * Override so that the bonds (child objects) have their render size
     * adjusted too
     */
    void setRenderSize(const QSize& renderSize);

    /**
     * Statically render the atom, for MoleculePreviewItem
     */
    static QPixmap renderAtom(KGameRenderer* renderer, atom at, int size);

    // enable use of qgraphicsitem_cast
    enum { Type = UserType + 2 };
    int type() const override { return Type; }
private:
    // from molecule
    int m_atomNum;

    static QHash<char, QString> s_names; // cryptic_char -> elemName
    static QHash<char, QString> s_bondNames; // cryptic_char -> bondName

    /**
     * Creates hashes for translating atom and bond signatures found in
     * level files to corresponding SVG-element names
     */
    static void fillNameHashes();
};

class QTimeLine;
/**
 *  FieldItem that represents clickable arrow.
 *  While showing plays nice fade-in effect
 */
class ArrowFieldItem : public QObject, public FieldItem
{
    Q_OBJECT
public:
    explicit ArrowFieldItem( KGameRenderer* renderer, PlayField::Direction dir, QGraphicsScene* scene );
    ~ArrowFieldItem() override;

    // enable use of qgraphicsitem_cast
    enum { Type = UserType + 3 };
    int type() const override { return Type; }
private Q_SLOTS:
    void setOpacity( qreal opacity );
private:
    QVariant itemChange( GraphicsItemChange change, const QVariant& value ) override;

    /**
     *  Timeline object to control fade-in animation
     */
    QTimeLine *m_timeLine;
};

class Molecule;
class PlayField;

/**
 *  QGraphicsItem which displays molecule preview.
 */
class MoleculePreviewItem : public QGraphicsItem
{
public:
    explicit MoleculePreviewItem( PlayField* scene );
    ~MoleculePreviewItem() override;

    /**
     *  Sets molecule to display
     */
    void setMolecule( const Molecule* mol );

    /**
     *  Sets item width. Height will be calculated automatically
     */
    void setWidth( int width );
    /**
     *  Sets maximum atom size in rendered molecule preview.
     *  Usually atom size is calculated so the whole molecule can fit
     *  in the item.
     *  In some cases - when item width is big and the molecule is small this
     *  can lead to preview having a huge molecule with atom size larger than
     *  in playfield :). That looks not very good, hence this function.
     */
    void setMaxAtomSize( int maxSize );

    inline QRectF boundingRect() const override { return QRectF(0,0, m_width, m_width); } // reimp
private:
    void paint( QPainter * painter, const QStyleOptionGraphicsItem*, QWidget * widget = nullptr ) override;

    KGameRenderer* m_renderer;
    int m_width;
    int m_atomSize;
    int m_maxAtomSize;
    const Molecule* m_mol;
};

#endif
