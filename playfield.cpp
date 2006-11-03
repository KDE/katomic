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
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QResizeEvent>

#include <kstandarddirs.h>
#include <ksimpleconfig.h>

#include "katomicrenderer.h"
#include "playfield.h"
#include "molek.h"
#include "atom.h"

class FieldGraphicsItem : public QGraphicsPixmapItem
{
public:
    FieldGraphicsItem( QGraphicsScene* scene )
        : QGraphicsPixmapItem( 0, scene ), m_fieldX(0), m_fieldY(0), m_atomNum(0)
    { }
    void setFieldX(int x) { m_fieldX = x; }
    void setFieldY(int y) { m_fieldY = y; }
    void setFieldXY(int x, int y) { m_fieldX = x; m_fieldY = y; }

    void setAtomNum(int n) { m_atomNum = n; }

    int fieldX() const { return m_fieldX; }
    int fieldY() const { return m_fieldY; }
    int atomNum() const { return m_atomNum; }
private:
    int m_fieldX;
    int m_fieldY;
    // from molecule
    // not meaningful for arrows
    int m_atomNum; 
};

PlayFieldView::PlayFieldView( PlayField* field, QWidget* parent )
    : QGraphicsView(field, parent), m_playField(field)
{

}

void PlayFieldView::resizeEvent( QResizeEvent* ev )
{
    m_playField->resize( ev->size().width(), ev->size().height() );
}

// =============== Play Field ========================

PlayField::PlayField( QObject* parent )
    : QGraphicsScene(parent), m_mol(0), m_elemSize(30), m_selX(-1), m_selY(-1)
{
    m_renderer = new KAtomicRenderer( KStandardDirs::locate("appdata", "pics/default_theme.svgz"), this );
    m_renderer->setElementSize( m_elemSize );

    m_upArrow = new FieldGraphicsItem(this);
    m_downArrow = new FieldGraphicsItem(this);
    m_leftArrow = new FieldGraphicsItem(this);
    m_rightArrow = new FieldGraphicsItem(this);
    updateArrows(); // this will hide them

    resize( FIELD_SIZE*m_elemSize, FIELD_SIZE*m_elemSize );
}

void PlayField::loadLevel(const KSimpleConfig& config)
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
            bool wall = false;
            if(c.isDigit())
            {
                FieldGraphicsItem* atom = new FieldGraphicsItem(this);
                atom->setFieldXY(i, j);
                atom->setAtomNum(QString(c).toInt());

                m_atoms.append(atom);
                //pixmaps will be set in resize
            }
            else if( c == '#' )
                wall = true;

            m_field[i][j] = wall;
        }
    }

    m_selX = m_selY = -1;
    updateArrows(); // this will hide them (no atom selected)
    updateFieldItems();
    //nextAtom();
}

void PlayField::updateFieldItems()
{
    foreach( FieldGraphicsItem *item, m_atoms )
    {
        item->setPixmap( m_renderer->renderAtom( m_mol->getAtom(item->atomNum()) ) );
        item->setPos( item->fieldX()*m_elemSize, item->fieldY()*m_elemSize );
        item->show();
    }

    m_upArrow->setPixmap( m_renderer->renderNonAtom('^') );
    m_upArrow->setPos( m_upArrow->fieldX()*m_elemSize, m_upArrow->fieldY()*m_elemSize );

    m_downArrow->setPixmap( m_renderer->renderNonAtom('_') );
    m_downArrow->setPos( m_downArrow->fieldX()*m_elemSize, m_downArrow->fieldY()*m_elemSize );

    m_leftArrow->setPixmap( m_renderer->renderNonAtom('<') );
    m_leftArrow->setPos( m_leftArrow->fieldX()*m_elemSize, m_leftArrow->fieldY()*m_elemSize );

    m_rightArrow->setPixmap( m_renderer->renderNonAtom('>') );
    m_rightArrow->setPos( m_rightArrow->fieldX()*m_elemSize, m_rightArrow->fieldY()*m_elemSize );
}

void PlayField::resize( int width, int height)
{
    setSceneRect( 0, 0, width, height );
    m_elemSize = qMin(width, height) / FIELD_SIZE;
    m_renderer->setElementSize( m_elemSize );
    kDebug() << "elemSize:" << m_elemSize << endl;
    updateFieldItems();
}

void PlayField::mousePressEvent( QGraphicsSceneMouseEvent* ev )
{
    int fx = static_cast<int>( ev->scenePos().x() / m_elemSize );
    int fy = static_cast<int>( ev->scenePos().y() / m_elemSize );

    foreach( FieldGraphicsItem* item, m_atoms )
    {
        if( item->fieldX() == fx && item->fieldY() == fy )
        {
            m_selX = fx;
            m_selY = fy;
            updateArrows();
        }
    }
}

void PlayField::updateArrows()
{
    m_upArrow->hide();
    m_downArrow->hide();
    m_leftArrow->hide();
    m_rightArrow->hide();

    if(m_selX == -1 || m_selY == -1)
        return;
    if(m_field[m_selX-1][m_selY] == false) // i.e. no wall
    {
        m_leftArrow->show();
        m_leftArrow->setFieldXY( m_selX-1, m_selY );
        m_leftArrow->setPos( (m_selX-1)*m_elemSize, m_selY*m_elemSize );
    }
    if(m_field[m_selX+1][m_selY] == false)
    {
        m_rightArrow->show();
        m_rightArrow->setFieldXY( m_selX+1, m_selY );
        m_rightArrow->setPos( (m_selX+1)*m_elemSize, m_selY*m_elemSize );
    }
    if(m_field[m_selX][m_selY-1] == false)
    {
        m_upArrow->show();
        m_upArrow->setFieldXY( m_selX, m_selY-1 );
        m_upArrow->setPos( m_selX*m_elemSize, (m_selY-1)*m_elemSize );
    }
    if(m_field[m_selX][m_selY+1] == false)
    {
        m_downArrow->show();
        m_downArrow->setFieldXY( m_selX, m_selY+1 );
        m_downArrow->setPos( m_selX*m_elemSize, (m_selY+1)*m_elemSize );
    }
}

void PlayField::drawBackground( QPainter *p, const QRectF& r)
{
    p->fillRect(r, Qt::black);

    kDebug() << " -= begin paint event 1=- " << endl;
    QPixmap aPix = m_renderer->renderNonAtom('#');
    for (int i = 0; i < FIELD_SIZE; i++)
        for (int j = 0; j < FIELD_SIZE; j++)
            // FIXME dimsuz: move away from all this digits! :)
            if (m_field[i][j])
                p->drawPixmap(i*m_elemSize, j*m_elemSize, aPix);
    kDebug() << "-= end paint event =-" << endl;
}
