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
#include <QPainter>
#include <QTimeLine>
#include <kdebug.h>
#include "fielditem.h"
#include "molecule.h"

ArrowFieldItem::ArrowFieldItem( QGraphicsScene* scene )
    : FieldItem(scene), m_opacity(0.0)
{
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
    m_opacity = opacity;
    update();
}

void ArrowFieldItem::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget )
{
    painter->setOpacity( m_opacity );
    QGraphicsPixmapItem::paint( painter, option, widget );
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

MoleculePreviewItem::MoleculePreviewItem( QGraphicsScene* scene )
    : QGraphicsItem( 0, scene ), m_width(0), m_maxAtomSize(30)
{
    m_molRenderer = new MoleculeRenderer();
}

MoleculePreviewItem::~MoleculePreviewItem()
{
    delete m_molRenderer;
}

void MoleculePreviewItem::setMolecule( const Molecule* mol )
{
    m_molRenderer->setMolecule(mol);
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

    int w = m_molRenderer->molecule()->width();
    int h = m_molRenderer->molecule()->height();
    int atomSize = width / qMax(w,h);
    m_molRenderer->setAtomSize( qMin(atomSize, m_maxAtomSize) );
    update();
}

void MoleculePreviewItem::paint( QPainter * painter, const QStyleOptionGraphicsItem*, QWidget *)
{
    painter->setBrush(Qt::black);
    painter->setOpacity(0.5);
    painter->drawRect(boundingRect());
    painter->setOpacity(1.0);
    m_molRenderer->render(painter, QPoint(m_width/2, m_width/2) );
}

#include "fielditem.moc"
