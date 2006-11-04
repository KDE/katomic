/*******************************************************************
 *
 * Copyright 2006 Dmitry Suzdalev <dimsuz@gmail.com>
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
#include <QTimeLine>

#include <kstandarddirs.h>
#include <ksimpleconfig.h>

#include "katomicrenderer.h"
#include "playfield.h"
#include "molecule.h"
#include "atom.h"

class FieldGraphicsItem : public QGraphicsPixmapItem
{
public:
    FieldGraphicsItem( QGraphicsScene* scene )
        : QGraphicsPixmapItem( 0, scene ), m_fieldX(0), m_fieldY(0), m_atomNum(-1)
    { setShapeMode( BoundingRectShape ); }
    void setFieldX(int x) { m_fieldX = x; }
    void setFieldY(int y) { m_fieldY = y; }
    void setFieldXY(int x, int y) { m_fieldX = x; m_fieldY = y; }

    void setAtomNum(int n) { m_atomNum = n; }

    int fieldX() const { return m_fieldX; }
    int fieldY() const { return m_fieldY; }
    int atomNum() const { return m_atomNum; }

    // enable use of qgraphicsitem_cast
    enum { Type = UserType + 1 };
    virtual int type() const { return Type; }
private:
    int m_fieldX;
    int m_fieldY;
    // from molecule
    // -1 for arrows
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
    : QGraphicsScene(parent), m_mol(0), m_numMoves(0), m_elemSize(30), m_selAtom(0)
{
    m_renderer = new KAtomicRenderer( KStandardDirs::locate("appdata", "pics/default_theme.svgz"), this );
    m_renderer->setElementSize( m_elemSize );

    m_timeLine = new QTimeLine(300, this);
    connect(m_timeLine, SIGNAL(frameChanged(int)), SLOT(animFrameChanged(int)) );

    m_upArrow = new FieldGraphicsItem(this);
    m_downArrow = new FieldGraphicsItem(this);
    m_leftArrow = new FieldGraphicsItem(this);
    m_rightArrow = new FieldGraphicsItem(this);
    updateArrows(true); // this will hide them

    resize( FIELD_SIZE*m_elemSize, FIELD_SIZE*m_elemSize );
}

void PlayField::loadLevel(const KSimpleConfig& config)
{
    qDeleteAll(m_atoms);
    m_atoms.clear();
    m_numMoves = 0;

    m_mol->load(config);

    QString key;

    for (int j = 0; j < FIELD_SIZE; j++) {

        key.sprintf("feld_%02d", j);
        QString line = config.readEntry(key,QString());

        for (int i = 0; i < FIELD_SIZE; i++)
        {
            QChar c = line.at(i);
            bool wall = false;
            if(c == '.')
            {
                wall = false;
            }
            else if( c == '#' )
            {
                wall = true;
            }
            else //atom
            {
                FieldGraphicsItem* atom = new FieldGraphicsItem(this);
                atom->setFieldXY(i, j);
                atom->setAtomNum(atom2int(c.toLatin1()));

                m_atoms.append(atom);
                //pixmaps will be set in updateFieldItems
            }

            m_field[i][j] = wall;
        }
    }

    m_selAtom = 0;
    updateArrows(true); // this will hide them (no atom selected)
    updateFieldItems();
    nextAtom();
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
    kDebug() << "resize:" << width << "," << height << endl;
    setSceneRect( 0, 0, width, height );
    m_elemSize = qMin(width, height) / FIELD_SIZE;
    m_renderer->setElementSize( m_elemSize );
    updateFieldItems();
}

void PlayField::nextAtom()
{
    if(!m_selAtom)
    {
        m_selAtom = m_atoms.at(0);
        updateArrows();
        return;
    }

    int xs = m_selAtom->fieldX();
    int ys = m_selAtom->fieldY()+1;

    int x = xs;

    while(1)
    {
        FieldGraphicsItem* item = 0;
        for(int y=ys; y<FIELD_SIZE; ++y )
        {
            int px = x*m_elemSize+m_elemSize/2;
            int py = y*m_elemSize+m_elemSize/2;
            item = qgraphicsitem_cast<FieldGraphicsItem*>( itemAt(px, py) );
            if( item != 0 && item->atomNum() != -1 )
            {
                m_selAtom = item;
                updateArrows();
                return;
            }
        }
        x++;
        if(x==FIELD_SIZE)
            x = 0;
        ys=0;
    }
}

void PlayField::previousAtom()
{
    if(!m_selAtom)
    {
        m_selAtom = m_atoms.at(0);
        updateArrows();
        return;
    }

    int xs = m_selAtom->fieldX();
    int ys = m_selAtom->fieldY()-1;

    int x = xs;

    while(1)
    {
        FieldGraphicsItem* item = 0;
        for(int y=ys; y>=0; --y )
        {
            int px = x*m_elemSize+m_elemSize/2;
            int py = y*m_elemSize+m_elemSize/2;
            item = qgraphicsitem_cast<FieldGraphicsItem*>( itemAt(px, py) );
            if( item != 0 && item->atomNum() != -1 )
            {
                m_selAtom = item;
                updateArrows();
                return;
            }
        }
        x--;
        if(x==0)
            x = FIELD_SIZE-1;
        ys=FIELD_SIZE-1;
    }
}

void PlayField::mousePressEvent( QGraphicsSceneMouseEvent* ev )
{
    if( m_timeLine->state() == QTimeLine::Running )
        return;

    FieldGraphicsItem *clickedItem = qgraphicsitem_cast<FieldGraphicsItem*>(itemAt(ev->scenePos()));
    if(!clickedItem)
        return;

    if( m_atoms.indexOf( clickedItem ) != -1 ) // that is: atom selected
    {
        m_selAtom = clickedItem;
        updateArrows();
    }
    else if( clickedItem == m_upArrow )
    {
        moveSelectedAtom( Up );
    }
    else if( clickedItem == m_downArrow )
    {
        moveSelectedAtom( Down );
    }
    else if( clickedItem == m_rightArrow )
    {
        moveSelectedAtom( Right );
    }
    else if( clickedItem == m_leftArrow )
    {
        moveSelectedAtom( Left );
    }
}

void PlayField::moveSelectedAtom( Direction dir )
{
    if( m_timeLine->state() == QTimeLine::Running )
        return;

    // helpers
    int x = 0, y = 0;
    int selX = m_selAtom->fieldX();
    int selY = m_selAtom->fieldY();
    int numEmptyCells=0;

    switch( dir )
    {
        case Up:
            y = selY;
            m_dir = Up;
            while( cellIsEmpty(selX, --y) )
                numEmptyCells++;
            break;
        case Down:
            y = selY;
            m_dir = Down;
            while( cellIsEmpty(selX, ++y) )
                numEmptyCells++;
            break;
        case Left:
            x = selX;
            m_dir = Left;
            while( cellIsEmpty(--x, selY) )
                numEmptyCells++;
            break;
        case Right:
            x = selX;
            m_dir = Right;
            while( cellIsEmpty(++x, selY) )
                numEmptyCells++;
            break;
    }

    if( numEmptyCells == 0)
        return;

    m_timeLine->setCurrentTime(0); // reset
    m_timeLine->setDuration( numEmptyCells * 150 ); // 1cell/150msec speed
    m_timeLine->setFrameRange( 0, numEmptyCells*m_elemSize ); // 1frame=1pixel
    updateArrows(true); // hide them
    m_timeLine->start();
}

// FIXME dimsuz: write fieldtoPix and pixtofield helpers and use them everywhere
// in this case if i change location of the field there will be one place to change
void PlayField::animFrameChanged(int frame)
{
    int pos=0;
    switch( m_dir )
    {
        case Up:
            pos = m_selAtom->fieldY()*m_elemSize - frame;
            m_selAtom->setPos( m_selAtom->x(), pos );
            break;
        case Down:
            pos = m_selAtom->fieldY()*m_elemSize + frame;
            m_selAtom->setPos( m_selAtom->x(), pos );
            break;
        case Left:
            pos = m_selAtom->fieldX()*m_elemSize - frame;
            m_selAtom->setPos( pos, m_selAtom->y() );
            break;
        case Right:
            pos = m_selAtom->fieldX()*m_elemSize + frame;
            m_selAtom->setPos( pos, m_selAtom->y() );
            break;
    }
    if(frame == m_timeLine->endFrame()) // that is: move finished
    {
        m_selAtom->setFieldX( static_cast<int>(m_selAtom->pos().x()/m_elemSize) );
        m_selAtom->setFieldY( static_cast<int>(m_selAtom->pos().y()/m_elemSize) );
        updateArrows();
        m_numMoves++;
        if(checkDone())
            emit gameOver(m_numMoves);
    }
}

// most complicated algorithm ;-)
bool PlayField::checkDone() const
{
    // let's assume that molecule is done
    // first we find molecule origin in field coords
    // by finding minimum fieldX, fieldY through all atoms
    int minX = FIELD_SIZE+1;
    int minY = FIELD_SIZE+1;
    foreach( FieldGraphicsItem* atom, m_atoms )
    {
        if(atom->fieldX() < minX)
            minX = atom->fieldX();
        if(atom->fieldY() < minY)
            minY = atom->fieldY();
    }
    // so origin is (minX,minY)
    // we'll substract this origin from each atom's coords and check
    // if the resulting position is the same as this atom has in molecule
    foreach( FieldGraphicsItem* atom, m_atoms )
    {
        uint atomNum = atom->atomNum();
        int molecCoordX = atom->fieldX() - minX;
        int molecCoordY = atom->fieldY() - minY;
        if( m_mol->getAtom( molecCoordX, molecCoordY ) != atomNum )
            return false; // nope. not there
    }
    return true;
}

bool PlayField::cellIsEmpty(int x, int y) const
{
    if(m_field[x][y] == true)
        return false; // it is a wall

    foreach( FieldGraphicsItem *atom, m_atoms )
    {
        if( atom->fieldX() == x && atom->fieldY() == y )
            return false;
    }
    return true;
}

void PlayField::updateArrows(bool justHide)
{
    m_upArrow->hide();
    m_downArrow->hide();
    m_leftArrow->hide();
    m_rightArrow->hide();

    if(justHide || !m_selAtom)
        return;

    int selX = m_selAtom->fieldX();
    int selY = m_selAtom->fieldY();

    if(cellIsEmpty(selX-1, selY))
    {
        m_leftArrow->show();
        m_leftArrow->setFieldXY( selX-1, selY );
        m_leftArrow->setPos( (selX-1)*m_elemSize, selY*m_elemSize );
    }
    if(cellIsEmpty(selX+1, selY))
    {
        m_rightArrow->show();
        m_rightArrow->setFieldXY( selX+1, selY );
        m_rightArrow->setPos( (selX+1)*m_elemSize, selY*m_elemSize );
    }
    if(cellIsEmpty(selX, selY-1))
    {
        m_upArrow->show();
        m_upArrow->setFieldXY( selX, selY-1 );
        m_upArrow->setPos( selX*m_elemSize, (selY-1)*m_elemSize );
    }
    if(cellIsEmpty(selX, selY+1))
    {
        m_downArrow->show();
        m_downArrow->setFieldXY( selX, selY+1 );
        m_downArrow->setPos( selX*m_elemSize, (selY+1)*m_elemSize );
    }
}

void PlayField::drawBackground( QPainter *p, const QRectF& r)
{
    m_renderer->renderBackground(p, r);

    QPixmap aPix = m_renderer->renderNonAtom('#');
    for (int i = 0; i < FIELD_SIZE; i++)
        for (int j = 0; j < FIELD_SIZE; j++)
            if (m_field[i][j])
                p->drawPixmap(i*m_elemSize, j*m_elemSize, aPix);
}

#include "playfield.moc"
