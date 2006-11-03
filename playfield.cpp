/*******************************************************************
 *
 * Copyright 2006 Dmitry Suzdalev <dimsuz@gmail.com>
 *
 * Parts of the code taken from 
 * feld.cpp, feld.h
 * Copyright Andreas Wuest <AndreasWuest@gmx.de>, Stephan Kulow <coolo@kde.org>
 *
 * This file is part of the KDE project "KAtomic"
 *
 * KReversi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * KReversi is distributed in the hope that it will be useful,
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
#include <QResizeEvent>
#include <QGraphicsPixmapItem>

#include <kstandarddirs.h>
#include <ksimpleconfig.h>

#include "katomicrenderer.h"
#include "playfield.h"
#include "molek.h"
#include "atom.h"

class AtomGraphicsItem : public QGraphicsPixmapItem
{
public:
    AtomGraphicsItem( QGraphicsScene* scene )
        : QGraphicsPixmapItem( 0, scene ), m_fieldX(0), m_fieldY(0)
    { }
    void setFieldX(int x) { m_fieldX = x; }
    void setFieldY(int y) { m_fieldY = y; }
    void setAtomNum(int n) { m_atomNum = n; }

    int fieldX() const { return m_fieldX; }
    int fieldY() const { return m_fieldY; }
    int atomNum() const { return m_atomNum; }
private:
    int m_fieldX;
    int m_fieldY;
    int m_atomNum; // from molecule
};

PlayFieldView::PlayFieldView( PlayField* field, QWidget* parent )
    : QGraphicsView(field, parent), m_playField(field)
{

}

void PlayFieldView::resizeEvent( QResizeEvent* ev )
{
    m_playField->resize( ev->size().width(), ev->size().height() );
}

PlayField::PlayField( QObject* parent )
    : QGraphicsScene(parent), m_mol(0), m_elemSize(30)
{
    m_renderer = new KAtomicRenderer( KStandardDirs::locate("appdata", "pics/default_theme.svgz"), this );
    m_renderer->setElementSize( m_elemSize );

    resize( FIELD_SIZE*m_elemSize, FIELD_SIZE*m_elemSize );
}

void PlayField::load (const KSimpleConfig& config)
{
    qDeleteAll(m_atoms);
    m_atoms.clear();

    m_mol->load(config);

    QString key;

    for (int j = 0; j < FIELD_SIZE; j++) {

        key.sprintf("feld_%02d", j);
        QString line = config.readEntry(key,QString());

        for (int i = 0; i < FIELD_SIZE; i++)
        {
            QChar c = line.at(i);
            char fc = 0; // non 0 only in case of '#'
            if(c.isDigit())
            {
                AtomGraphicsItem* atom = new AtomGraphicsItem(this);
                atom->setFieldX(i);
                atom->setFieldY(j);
                atom->setAtomNum(QString(c).toInt());

                m_atoms.append(atom);
                //pixmaps will be set in resize
            }
            else if( c == '#' )
                fc = '#';

            m_field[i][j] = fc;
        }
    }

    renderAtoms();
    //nextAtom();
}

void PlayField::renderAtoms()
{
    foreach( AtomGraphicsItem *item, m_atoms )
    {
        item->setPixmap( m_renderer->renderAtom( m_mol->getAtom(item->atomNum()) ) );
        item->setPos( item->fieldX()*m_elemSize, item->fieldY()*m_elemSize );
        kDebug() << "setting pos: " << item->fieldX() << "," << item->fieldY() << endl;
        item->show();
    }
    kDebug() << "num atoms: " << m_atoms.count() << endl;
}

void PlayField::resize( int width, int height)
{
    setSceneRect( 0, 0, width, height );
    m_elemSize = qMin(width, height) / FIELD_SIZE;
    m_renderer->setElementSize( m_elemSize );
    kDebug() << "elemSize:" << m_elemSize << endl;
    renderAtoms();
}

void PlayField::drawBackground( QPainter *p, const QRectF& r)
{
    p->fillRect(r, Qt::black);

    kDebug() << " -= begin paint event 1=- " << endl;
    QPixmap aPix = m_renderer->renderNonAtom('#');
    for (int i = 0; i < FIELD_SIZE; i++)
        for (int j = 0; j < FIELD_SIZE; j++)
            // FIXME dimsuz: move away from all this digits! :)
            if (m_field[i][j] == '#')
                p->drawPixmap(i*m_elemSize, j*m_elemSize, aPix);
    kDebug() << "-= end paint event =-" << endl;
}
