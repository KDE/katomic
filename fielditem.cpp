/*******************************************************************
 *
 * Copyright 2006 Dmitry Suzdalev <dimsuz@gmail.com>
 * Copyright 2007 Carsten Niehaus <cniehaus@kde.org>
 * Copyright 2010 Brian Croom <brian.s.croom@gmail.com>
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

#include "fielditem.h"

#include <QPainter>
#include <QTimeLine>

#include <kdebug.h>
#include <klocale.h>
#include "molecule.h"
#include "playfield.h"

FieldItem::FieldItem( KGameRenderer* renderer, const QString& spriteKey, QGraphicsScene* scene )
    : KGameRenderedItem( renderer, spriteKey ), m_fieldX(0), m_fieldY(0)
{
    if( scene )
        scene->addItem( this );
    setShapeMode( BoundingRectShape );
}


static const char* arrow_spriteKeys[] = {"arrow_Up", "arrow_Down", "arrow_Left", "arrow_Right"};

ArrowFieldItem::ArrowFieldItem( KGameRenderer* renderer, PlayField::Direction dir, QGraphicsScene* scene )
    : FieldItem( renderer, arrow_spriteKeys[dir], scene )
{
    setOpacity(0.0); //start invisible
    m_timeLine = new QTimeLine(200);
    m_timeLine->setFrameRange( 0, 30 );
    connect(m_timeLine, SIGNAL(valueChanged(qreal)), SLOT(setOpacity(qreal)) );
}

ArrowFieldItem::~ArrowFieldItem()
{
    delete m_timeLine;
}

void ArrowFieldItem::setOpacity( qreal opacity )
{
    //NOTE: This method is only there because QGI::setOpacity is not a slot.
    QGraphicsItem::setOpacity(opacity);
}

QVariant ArrowFieldItem::itemChange( GraphicsItemChange change, const QVariant& value )
{
    if(change == ItemVisibleChange)
    {
        if(value.toBool())
        {
            m_timeLine->stop();
            m_timeLine->setCurrentTime(0);
            m_timeLine->start();
        }
    }
    return value;
}



AtomFieldItem::AtomFieldItem( KGameRenderer* renderer, const atom& at, QGraphicsScene* scene )
    : FieldItem(renderer, s_names.value(at.obj), scene), m_atomNum(-1)
{
    if(s_names.empty())
    {
        fillNameHashes();
        setSpriteKey(s_names.value(at.obj)); // It wasn't yet filled in when the constructor was called
    }

    // create the bonds as child items
    for (int c = 0; c < MAX_CONNS_PER_ATOM; c++)
    {
        char conn = at.conn[c];
        if (!conn)
            break;

        KGameRenderedItem* bond = new KGameRenderedItem(renderer, s_bondNames.value(conn), this);
        bond->setFlag(QGraphicsItem::ItemStacksBehindParent);
    }
}

void AtomFieldItem::setRenderSize(const QSize& renderSize)
{
    KGameRenderedItem::setRenderSize(renderSize);

    QList<QGraphicsItem*> bonds = childItems();
    foreach(QGraphicsItem* item, bonds)
    {
        dynamic_cast<KGameRenderedItem*>(item)->setRenderSize(renderSize);
    }
}

QHash<char, QString> AtomFieldItem::s_names;
QHash<char, QString> AtomFieldItem::s_bondNames;

void AtomFieldItem::fillNameHashes()
{
    s_names['1'] = "atom_H";
    s_names['2'] = "atom_C";
    s_names['3'] = "atom_O";
    s_names['4'] = "atom_N";
    s_names['5'] = "atom_S";
    s_names['6'] = "atom_F";
    s_names['7'] = "atom_Cl";
    s_names['8'] = "atom_Br";
    s_names['9'] = "atom_P";
    s_names['0'] = "atom_J";
    s_names['o'] = "atom_Crystal";
    s_names['A'] = "connector_Hor";
    s_names['B'] = "connector_Slash";
    s_names['C'] = "connector_Ver";
    s_names['D'] = "connector_Backslash";
    s_names['#'] = "wall";
    s_names['<'] = "arrow_Left";
    s_names['>'] = "arrow_Right";
    s_names['^'] = "arrow_Up";
    s_names['_'] = "arrow_Down";
    s_names['E'] = "atom_flask0";
    s_names['F'] = "atom_flask1";
    s_names['G'] = "atom_flask2";
    s_names['H'] = "atom_flask3";
    s_names['I'] = "atom_flask4";
    s_names['J'] = "atom_flask5";
    s_names['K'] = "atom_flask6";
    s_names['L'] = "atom_flask7";

    s_bondNames['a'] = "bond_I_Top";
    s_bondNames['b'] = "bond_I_TopRight";
    s_bondNames['c'] = "bond_I_Right";
    s_bondNames['d'] = "bond_I_BotRight";
    s_bondNames['e'] = "bond_I_Bottom";
    s_bondNames['f'] = "bond_I_BotLeft";
    s_bondNames['g'] = "bond_I_Left";
    s_bondNames['h'] = "bond_I_TopLeft";

    s_bondNames['A'] = "bond_II_Top";
    s_bondNames['B'] = "bond_II_Right";
    s_bondNames['C'] = "bond_II_Bottom";
    s_bondNames['D'] = "bond_II_Left";

    s_bondNames['E'] = "bond_III_Top";
    s_bondNames['F'] = "bond_III_Right";
    s_bondNames['G'] = "bond_III_Bottom";
    s_bondNames['H'] = "bond_III_Left";
}

QPixmap AtomFieldItem::renderAtom( KGameRenderer* renderer, const atom& at, int size )
{
    if (size == 0) return QPixmap();

    QPixmap atomPix = renderer->spritePixmap(s_names.value(at.obj), QSize(size, size));

    QPainter p;
    QPixmap bonds(size,size);
    bonds.fill(Qt::transparent);
    for (int c = 0; c < MAX_CONNS_PER_ATOM; c++)
    {
        char conn = at.conn[c];
        if (!conn)
            break;

        QPixmap pix = renderer->spritePixmap(s_bondNames.value(conn), QSize(size, size));

        p.begin(&bonds);
        p.drawPixmap(0,0, pix);
        p.end();
    }

    p.begin(&bonds);
    p.drawPixmap(0,0, atomPix);
    p.end();
    return bonds;
}

// ----------------- MoleculePreviewItem ----------------------------

MoleculePreviewItem::MoleculePreviewItem( PlayField* scene )
    : QGraphicsItem( 0 ), m_renderer(scene->renderer()), m_width(0),
      m_atomSize(20), m_maxAtomSize(30), m_mol( 0 )
{
   scene->addItem(this);
}

MoleculePreviewItem::~MoleculePreviewItem()
{
}

void MoleculePreviewItem::setMolecule( const Molecule* mol )
{
    m_mol = mol;
    setWidth( m_width ); // trigger atom size update
}

void MoleculePreviewItem::setMaxAtomSize(int maxSize)
{
    m_maxAtomSize = maxSize;
    setWidth( m_width ); // trigger atom size update
}

void MoleculePreviewItem::setWidth(int width)
{
    m_width = width;

    if(!m_mol)
        return;
    int w = m_mol->width();
    int h = m_mol->height();
    int atomSize = width / qMax(w,h);
    m_atomSize = qMin(atomSize, m_maxAtomSize);
    update();
}

void MoleculePreviewItem::paint( QPainter * painter, const QStyleOptionGraphicsItem*, QWidget *)
{
    if ( m_width == 0 || m_mol == 0 )
        return;

    painter->save();
    painter->setBrush(Qt::gray);
    painter->setOpacity(0.5);
    painter->drawRect(boundingRect());
    painter->setOpacity(1.0);

    int originX = m_width/2 - m_atomSize*m_mol->width()/2;
    int originY = m_width/2 - m_atomSize*m_mol->height()/2;

    // Paint the playing field
    for (int i = 0; i < MOLECULE_SIZE; i++)
        for (int j = 0; j < MOLECULE_SIZE; j++)
        {
            int x = originX + i * m_atomSize;
            int y = originY + j * m_atomSize;

            if (m_mol->getAtom(i,j) == 0)
                continue;

            int atomIdx = m_mol->getAtom(i,j);
            QPixmap aPix = AtomFieldItem::renderAtom(m_renderer, m_mol->getAtom(atomIdx), m_atomSize);
            painter->drawPixmap(x, y, aPix);
        }
    painter->restore();
}

#include "fielditem.moc"
