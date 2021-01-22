/*
    This file is part of the KDE project "KAtomic"

    SPDX-FileCopyrightText: 2006 Dmitry Suzdalev <dimsuz@gmail.com>
    SPDX-FileCopyrightText: 2007 Carsten Niehaus <cniehaus@kde.org>
    SPDX-FileCopyrightText: 2010 Brian Croom <brian.s.croom@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "fielditem.h"

#include "molecule.h"

#include <QPainter>
#include <QTimeLine>

FieldItem::FieldItem( KGameRenderer* renderer, const QString& spriteKey, QGraphicsScene* scene )
    : KGameRenderedItem( renderer, spriteKey ), m_fieldX(0), m_fieldY(0)
{
    if( scene )
        scene->addItem( this );
    setShapeMode( BoundingRectShape );
}


static const char* arrow_spriteKeys[] = {"arrow_Up", "arrow_Down", "arrow_Left", "arrow_Right"};

ArrowFieldItem::ArrowFieldItem( KGameRenderer* renderer, PlayField::Direction dir, QGraphicsScene* scene )
    : FieldItem( renderer, QLatin1String(arrow_spriteKeys[dir]), scene )
{
    setOpacity(0.0); //start invisible
    m_timeLine = new QTimeLine(200);
    m_timeLine->setFrameRange( 0, 30 );
    connect(m_timeLine, &QTimeLine::valueChanged, this, &ArrowFieldItem::setOpacity);
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



AtomFieldItem::AtomFieldItem( KGameRenderer* renderer, atom at, QGraphicsScene* scene )
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

    const QList<QGraphicsItem*> bonds = childItems();
    for (QGraphicsItem* item : bonds)
    {
        dynamic_cast<KGameRenderedItem*>(item)->setRenderSize(renderSize);
    }
}

QHash<char, QString> AtomFieldItem::s_names;
QHash<char, QString> AtomFieldItem::s_bondNames;

void AtomFieldItem::fillNameHashes()
{
    s_names['1'] = QStringLiteral("atom_H");
    s_names['2'] = QStringLiteral("atom_C");
    s_names['3'] = QStringLiteral("atom_O");
    s_names['4'] = QStringLiteral("atom_N");
    s_names['5'] = QStringLiteral("atom_S");
    s_names['6'] = QStringLiteral("atom_F");
    s_names['7'] = QStringLiteral("atom_Cl");
    s_names['8'] = QStringLiteral("atom_Br");
    s_names['9'] = QStringLiteral("atom_P");
    s_names['0'] = QStringLiteral("atom_J");
    s_names['o'] = QStringLiteral("atom_Crystal");
    s_names['A'] = QStringLiteral("connector_Hor");
    s_names['B'] = QStringLiteral("connector_Slash");
    s_names['C'] = QStringLiteral("connector_Ver");
    s_names['D'] = QStringLiteral("connector_Backslash");
    s_names['#'] = QStringLiteral("wall");
    s_names['<'] = QStringLiteral("arrow_Left");
    s_names['>'] = QStringLiteral("arrow_Right");
    s_names['^'] = QStringLiteral("arrow_Up");
    s_names['_'] = QStringLiteral("arrow_Down");
    s_names['E'] = QStringLiteral("atom_flask0");
    s_names['F'] = QStringLiteral("atom_flask1");
    s_names['G'] = QStringLiteral("atom_flask2");
    s_names['H'] = QStringLiteral("atom_flask3");
    s_names['I'] = QStringLiteral("atom_flask4");
    s_names['J'] = QStringLiteral("atom_flask5");
    s_names['K'] = QStringLiteral("atom_flask6");
    s_names['L'] = QStringLiteral("atom_flask7");

    s_bondNames['a'] = QStringLiteral("bond_I_Top");
    s_bondNames['b'] = QStringLiteral("bond_I_TopRight");
    s_bondNames['c'] = QStringLiteral("bond_I_Right");
    s_bondNames['d'] = QStringLiteral("bond_I_BotRight");
    s_bondNames['e'] = QStringLiteral("bond_I_Bottom");
    s_bondNames['f'] = QStringLiteral("bond_I_BotLeft");
    s_bondNames['g'] = QStringLiteral("bond_I_Left");
    s_bondNames['h'] = QStringLiteral("bond_I_TopLeft");

    s_bondNames['A'] = QStringLiteral("bond_II_Top");
    s_bondNames['B'] = QStringLiteral("bond_II_Right");
    s_bondNames['C'] = QStringLiteral("bond_II_Bottom");
    s_bondNames['D'] = QStringLiteral("bond_II_Left");

    s_bondNames['E'] = QStringLiteral("bond_III_Top");
    s_bondNames['F'] = QStringLiteral("bond_III_Right");
    s_bondNames['G'] = QStringLiteral("bond_III_Bottom");
    s_bondNames['H'] = QStringLiteral("bond_III_Left");
}

QPixmap AtomFieldItem::renderAtom( KGameRenderer* renderer, atom at, int size )
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
    : QGraphicsItem( nullptr ), m_renderer(scene->renderer()), m_width(0),
    m_atomSize(20), m_maxAtomSize(30), m_mol( nullptr )
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
    if ( m_width == 0 || m_mol == nullptr )
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


