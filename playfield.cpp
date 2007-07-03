/*******************************************************************
 *
 * Copyright 2006-2007 Dmitry Suzdalev <dimsuz@gmail.com>
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

#include "playfield.h"

#include <QGraphicsSceneMouseEvent>
#include <QResizeEvent>
#include <QTimeLine>
#include <QPainter>

#include <KStandardDirs>
#include <KConfig>

#include <KGamePopupItem>

#include "katomicrenderer.h"
#include "molecule.h"
#include "fielditem.h"
#include "atom.h"

static const int MIN_INFO_SIZE=10;

PlayField::PlayField( QObject* parent )
    : QGraphicsScene(parent), m_mol(0), m_numMoves(0),
    m_elemSize(MIN_ELEM_SIZE), m_selIdx(-1), m_animSpeed(120),
    m_levelFinished(false)
{
    // this object will hold the current molecule
    m_mol = new Molecule();

    KAtomicRenderer::self()->restoreSavedBackground();

    m_atomTimeLine = new QTimeLine(300, this);
    connect(m_atomTimeLine, SIGNAL(frameChanged(int)), SLOT(atomAnimFrameChanged(int)) );

    m_upArrow = new ArrowFieldItem(this);
    m_downArrow = new ArrowFieldItem(this);
    m_leftArrow = new ArrowFieldItem(this);
    m_rightArrow = new ArrowFieldItem(this);

    m_messageItem = new KGamePopupItem();
    m_messageItem->setMessageOpacity(0.9);
    addItem(m_messageItem); // it hides itself by default

    m_previewItem = new MoleculePreviewItem(this);
    updateArrows(true); // this will hide them
}

PlayField::~PlayField()
{
    delete m_mol;
}

void PlayField::loadLevel(const KConfigGroup& config)
{
    qDeleteAll(m_atoms);
    m_atoms.clear();
    m_numMoves = 0;
    m_levelFinished = false;
    m_atomTimeLine->stop();

    m_undoStack.clear();
    m_redoStack.clear();
    emit enableUndo(false);
    emit enableRedo(false);

    m_mol->load(config);
    m_previewItem->setMolecule(m_mol);

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
                AtomFieldItem* atom = new AtomFieldItem(this);
                atom->setFieldXY(i, j);
                atom->setAtomNum(atom2int(c.toLatin1()));

                m_atoms.append(atom);
                //pixmaps will be set in updateFieldItems
            }

            m_field[i][j] = wall;
        }
    }

    m_selIdx = -1;
    updateArrows(true); // this will hide them (no atom selected)
    updateFieldItems();
    nextAtom();
}

void PlayField::updateFieldItems()
{
    foreach( AtomFieldItem *item, m_atoms )
    {
        item->setPixmap( KAtomicRenderer::self()->renderAtom( m_mol->getAtom(item->atomNum()), m_elemSize ) );

        // this may be true if resize happens during animation
        if( isAnimating() && m_selIdx != -1 && item == m_atoms.at(m_selIdx) )
            continue; // its position will be taken care of in atomAnimFrameChanged()

        item->setPos( toPixX( item->fieldX() ), toPixY( item->fieldY() ) );
        item->show();
    }

    m_upArrow->setPixmap( KAtomicRenderer::self()->renderNonAtom('^', m_elemSize) );
    m_upArrow->setPos( toPixX(m_upArrow->fieldX()), toPixY(m_upArrow->fieldY()) );

    m_downArrow->setPixmap( KAtomicRenderer::self()->renderNonAtom('_', m_elemSize ) );
    m_downArrow->setPos( toPixX(m_downArrow->fieldX()), toPixY(m_downArrow->fieldY()) );

    m_leftArrow->setPixmap( KAtomicRenderer::self()->renderNonAtom('<', m_elemSize ) );
    m_leftArrow->setPos( toPixX(m_leftArrow->fieldX()), toPixY(m_leftArrow->fieldY()) );

    m_rightArrow->setPixmap( KAtomicRenderer::self()->renderNonAtom('>', m_elemSize ) );
    m_rightArrow->setPos( toPixX(m_rightArrow->fieldX()), toPixY(m_rightArrow->fieldY()) );
}

void PlayField::resize( int width, int height)
{
    kDebug() << "resize:" << width << "," << height << endl;
    setSceneRect( 0, 0, width, height );

    // we take 1/4 of width for displaying preview
    int previewWidth = width/4;
    m_previewItem->setPos( width-previewWidth+2, 2 );
    m_previewItem->setWidth( previewWidth-4 );

    width -= previewWidth;

    int oldSize = m_elemSize;
    m_elemSize = qMin(width, height) / FIELD_SIZE;
    m_previewItem->setMaxAtomSize( m_elemSize );

    // if atom animation is running we need to rescale timeline
    if( isAnimating() )
    {
        kDebug() << "restarting animation" << endl;
        int curTime = m_atomTimeLine->currentTime();
        // calculate numCells to move using oldSize
        int numCells = m_atomTimeLine->endFrame()/oldSize;
        m_atomTimeLine->stop();
        // recalculate this with new m_elemSize
        m_atomTimeLine->setFrameRange( 0, numCells*m_elemSize );
        m_atomTimeLine->setCurrentTime(curTime);
        m_atomTimeLine->start();
    }
    updateFieldItems();
}

void PlayField::nextAtom()
{
    if ( m_levelFinished || isAnimating() )
        return;

    if(m_selIdx == -1)
    {
        m_selIdx = 0;
        updateArrows();
        return;
    }

    int xs = m_atoms.at(m_selIdx)->fieldX();
    int ys = m_atoms.at(m_selIdx)->fieldY()+1;

    int x = xs;

    while(1)
    {
        AtomFieldItem* item = 0;
        for(int y=ys; y<FIELD_SIZE; ++y )
        {
            int px = toPixX(x)+m_elemSize/2;
            int py = toPixY(y)+m_elemSize/2;
            item = qgraphicsitem_cast<AtomFieldItem*>( itemAt(px, py) );
            if( item != 0 )
            {
                m_selIdx = m_atoms.indexOf(item);
                updateArrows();
                // if this atom can't move, we won't return - we'll search further
                // until we found moveable one
                if( m_upArrow->isVisible() || m_rightArrow->isVisible()
                        || m_downArrow->isVisible() || m_leftArrow->isVisible() )
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
    if ( m_levelFinished || isAnimating() )
        return;

    if(m_selIdx == -1)
    {
        m_selIdx = 0;
        updateArrows();
        return;
    }

    int xs = m_atoms.at(m_selIdx)->fieldX();
    int ys = m_atoms.at(m_selIdx)->fieldY()-1;

    int x = xs;

    while(1)
    {
        AtomFieldItem* item = 0;
        for(int y=ys; y>=0; --y )
        {
            int px = toPixX(x)+m_elemSize/2;
            int py = toPixY(y)+m_elemSize/2;
            item = qgraphicsitem_cast<AtomFieldItem*>( itemAt(px, py) );
            if( item != 0 && item->atomNum() != -1 )
            {
                m_selIdx = m_atoms.indexOf(item);
                updateArrows();
                // if this atom can't move, we won't return - we'll search further
                // until we found moveable one
                if( m_upArrow->isVisible() || m_rightArrow->isVisible()
                        || m_downArrow->isVisible() || m_leftArrow->isVisible() )
                    return;
            }
        }
        x--;
        if(x==0)
            x = FIELD_SIZE-1;
        ys=FIELD_SIZE-1;
    }
}

void PlayField::undo()
{
    if( isAnimating() || m_undoStack.isEmpty())
        return;

    AtomMove am = m_undoStack.pop();
    if(m_redoStack.isEmpty())
        emit enableRedo(true);

    m_redoStack.push(am);

    if(m_undoStack.isEmpty())
        emit enableUndo(false);

    m_numMoves--;
    emit updateMoves(m_numMoves);

    m_selIdx = am.atomIdx;
    switch( am.dir )
    {
        case Up:
            moveSelectedAtom(Down, am.numCells);
            break;
        case Down:
            moveSelectedAtom(Up, am.numCells);
            break;
        case Left:
            moveSelectedAtom(Right, am.numCells);
            break;
        case Right:
            moveSelectedAtom(Left, am.numCells);
            break;
    }
}

void PlayField::redo()
{
    if( isAnimating() || m_redoStack.isEmpty() )
        return;

    AtomMove am = m_redoStack.pop();
    if(m_undoStack.isEmpty())
        emit enableUndo(true);

    if(!m_redoStack.isEmpty()) //otherwise it will be pushed at the end of the move
        m_undoStack.push(am);

    if(m_redoStack.isEmpty())
        emit enableRedo(false);

    m_numMoves++;
    emit updateMoves(m_numMoves);

    m_selIdx = am.atomIdx;
    moveSelectedAtom(am.dir, am.numCells);
}

void PlayField::undoAll()
{
    while( !m_undoStack.isEmpty() )
    {
        AtomMove am = m_undoStack.pop();
        m_redoStack.push( am );

        // adjust atom pos
        AtomFieldItem *atom = m_atoms.at(am.atomIdx);
        int xdelta = 0, ydelta = 0;
        switch(am.dir)
        {
            case Up:
                ydelta = am.numCells;
                break;
            case Down:
                ydelta = -am.numCells;
                break;
            case Right:
                xdelta = -am.numCells;
                break;
            case Left:
                xdelta = am.numCells;
                break;
        }
        atom->setFieldXY( atom->fieldX()+xdelta, atom->fieldY()+ydelta );
    }
    // update pixel positions
    foreach( AtomFieldItem* atom, m_atoms )
        atom->setPos( toPixX(atom->fieldX()), toPixY(atom->fieldY()));

    m_numMoves = 0;
    emit updateMoves(m_numMoves);
    emit enableUndo(false);
    emit enableRedo(!m_redoStack.isEmpty());
    m_selIdx = m_redoStack.last().atomIdx;
    updateArrows();
}

void PlayField::redoAll()
{
    while( !m_redoStack.isEmpty() )
    {
        AtomMove am = m_redoStack.pop();
        m_undoStack.push( am );

        // adjust atom pos
        AtomFieldItem *atom = m_atoms.at(am.atomIdx);
        int xdelta = 0, ydelta = 0;
        switch(am.dir)
        {
            case Up:
                ydelta = -am.numCells;
                break;
            case Down:
                ydelta = am.numCells;
                break;
            case Right:
                xdelta = am.numCells;
                break;
            case Left:
                xdelta = -am.numCells;
                break;
        }
        atom->setFieldXY( atom->fieldX()+xdelta, atom->fieldY()+ydelta );
    }
    // update pixel positions
    foreach( AtomFieldItem * atom, m_atoms )
        atom->setPos( toPixX(atom->fieldX()), toPixY(atom->fieldY()));

    m_numMoves = m_undoStack.count();
    emit updateMoves(m_numMoves);
    emit enableUndo(!m_undoStack.isEmpty());
    emit enableRedo(false);
    m_selIdx = m_undoStack.last().atomIdx;
    updateArrows();
}

void PlayField::mousePressEvent( QGraphicsSceneMouseEvent* ev )
{
    QGraphicsScene::mousePressEvent(ev);

    if( isAnimating() || m_levelFinished )
        return;

    QGraphicsItem* clickedItem = itemAt(ev->scenePos());
    if(!clickedItem)
        return;

    AtomFieldItem *atomItem = qgraphicsitem_cast<AtomFieldItem*>(clickedItem);
    if( atomItem ) // that is: atom selected
    {
        m_selIdx = m_atoms.indexOf( atomItem );
        updateArrows();
        return;
    }

    ArrowFieldItem *arrowItem = qgraphicsitem_cast<ArrowFieldItem*>(clickedItem);
    if( arrowItem == m_upArrow )
    {
        moveSelectedAtom( Up );
    }
    else if( arrowItem == m_downArrow )
    {
        moveSelectedAtom( Down );
    }
    else if( arrowItem == m_rightArrow )
    {
        moveSelectedAtom( Right );
    }
    else if( arrowItem == m_leftArrow )
    {
        moveSelectedAtom( Left );
    }
}

void PlayField::moveSelectedAtom( Direction dir, int numCells )
{
    if( isAnimating() )
        return;


    int numEmptyCells=0;
    m_dir = dir;

    // numCells is also a kind of indicator whether this
    // function was called interactively (=0) or from  undo/redo functions(!=0)
    if(numCells == 0) // then we'll calculate
    {
        // helpers
        int x = 0, y = 0;
        int selX = m_atoms.at(m_selIdx)->fieldX();
        int selY = m_atoms.at(m_selIdx)->fieldY();
        switch( dir )
        {
            case Up:
                y = selY;
                while( cellIsEmpty(selX, --y) )
                    numEmptyCells++;
                break;
            case Down:
                y = selY;
                while( cellIsEmpty(selX, ++y) )
                    numEmptyCells++;
                break;
            case Left:
                x = selX;
                while( cellIsEmpty(--x, selY) )
                    numEmptyCells++;
                break;
            case Right:
                x = selX;
                while( cellIsEmpty(++x, selY) )
                    numEmptyCells++;
                break;
        }
        // and clear the redo stack. we do it here
        // because if this function is called with numCells=0
        // this indicates it is called not from undo()/redo(),
        // but as a result of mouse/keyb input from player
        // so this is just a place to drop redo history :-)
        m_redoStack.clear();
        emit enableRedo(false);
        // only count it as move if we actually move :-)
        if(numEmptyCells)
            m_numMoves++;
    }
    else
        numEmptyCells = numCells;

    if( numEmptyCells == 0)
        return;

    // put undo info
    // don't put if we in the middle of series of undos
    if(m_redoStack.isEmpty())
    {
        if(m_undoStack.isEmpty())
            emit enableUndo(true);
        m_undoStack.push( AtomMove(m_selIdx, m_dir, numEmptyCells) );
    }

    m_atomTimeLine->setCurrentTime(0); // reset
    m_atomTimeLine->setDuration( numEmptyCells * m_animSpeed ); // 1cell/m_animSpeed speed
    m_atomTimeLine->setFrameRange( 0, numEmptyCells*m_elemSize ); // 1frame=1pixel
    updateArrows(true); // hide them
    m_atomTimeLine->start();
}

void PlayField::atomAnimFrameChanged(int frame)
{
    AtomFieldItem *selAtom = m_atoms.at(m_selIdx);
    int posx= toPixX(selAtom->fieldX());
    int posy= toPixY(selAtom->fieldY());

    switch( m_dir )
    {
        case Up:
            posy = toPixY(selAtom->fieldY()) - frame;
            break;
        case Down:
            posy = toPixY(selAtom->fieldY()) + frame;
            break;
        case Left:
            posx = toPixX(selAtom->fieldX()) - frame;
            break;
        case Right:
            posx = toPixX(selAtom->fieldX()) + frame;
            break;
    }

    selAtom->setPos(posx, posy);

    if(frame == m_atomTimeLine->endFrame()) // that is: move finished
    {
        // NOTE: consider moving this to separate function (something like moveFinished())
        // to improve code readablility
        selAtom->setFieldX( toFieldX((int)selAtom->pos().x()) );
        selAtom->setFieldY( toFieldY((int)selAtom->pos().y()) );
        updateArrows();

        emit updateMoves(m_numMoves);

        if(checkDone() && !m_levelFinished)
        {
            m_levelFinished = true;
            // hide arrows
            updateArrows(true);
            m_selIdx = -1;
            emit gameOver(m_numMoves);
        }
    }
}

// most complicated algorithm ;-)
bool PlayField::checkDone() const
{
    // let's assume that molecule is done
    // and see if we can break assumtion
    //
    // first we find molecule origin in field coords
    // by finding minimum fieldX, fieldY through all atoms
    int minX = FIELD_SIZE+1;
    int minY = FIELD_SIZE+1;
    foreach( AtomFieldItem* atom, m_atoms )
    {
        if(atom->fieldX() < minX)
            minX = atom->fieldX();
        if(atom->fieldY() < minY)
            minY = atom->fieldY();
    }
    // so origin is (minX,minY)
    // we'll substract this origin from each atom's coords and check
    // if the resulting position is the same as this atom has in molecule
    foreach( AtomFieldItem* atom, m_atoms )
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

    foreach( AtomFieldItem *atom, m_atoms )
    {
        if( atom->fieldX() == x && atom->fieldY() == y )
            return false;
    }
    return true;
}

void PlayField::setAnimationSpeed(int speed)
{
    if(speed == 0) // slow
        m_animSpeed = 300;
    else if (speed == 1) //normal
        m_animSpeed = 120;
    else
        m_animSpeed = 60;
}

void PlayField::updateArrows(bool justHide)
{
    m_upArrow->hide();
    m_downArrow->hide();
    m_leftArrow->hide();
    m_rightArrow->hide();

    if(justHide || m_selIdx == -1 || m_levelFinished)
        return;

    int selX = m_atoms.at(m_selIdx)->fieldX();
    int selY = m_atoms.at(m_selIdx)->fieldY();

    if(cellIsEmpty(selX-1, selY))
    {
        m_leftArrow->show();
        m_leftArrow->setFieldXY( selX-1, selY );
        m_leftArrow->setPos( toPixX(selX-1), toPixY(selY) );
    }
    if(cellIsEmpty(selX+1, selY))
    {
        m_rightArrow->show();
        m_rightArrow->setFieldXY( selX+1, selY );
        m_rightArrow->setPos( toPixX(selX+1), toPixY(selY) );
    }
    if(cellIsEmpty(selX, selY-1))
    {
        m_upArrow->show();
        m_upArrow->setFieldXY( selX, selY-1 );
        m_upArrow->setPos( toPixX(selX), toPixY(selY-1) );
    }
    if(cellIsEmpty(selX, selY+1))
    {
        m_downArrow->show();
        m_downArrow->setFieldXY( selX, selY+1 );
        m_downArrow->setPos( toPixX(selX), toPixY(selY+1) );
    }
}

void PlayField::drawBackground( QPainter *p, const QRectF&)
{
    p->drawPixmap(0, 0, KAtomicRenderer::self()->renderBackground( sceneRect().size().toSize() ));

    QPixmap aPix = KAtomicRenderer::self()->renderNonAtom('#', m_elemSize);
    for (int i = 0; i < FIELD_SIZE; i++)
        for (int j = 0; j < FIELD_SIZE; j++)
            if (m_field[i][j])
                p->drawPixmap(toPixX(i), toPixY(j), aPix);
}

bool PlayField::isAnimating() const
{
    return (m_atomTimeLine->state() == QTimeLine::Running);
}

void PlayField::saveGame( KConfigGroup& config ) const
{
    // REMEMBER: while saving use atom indexes within m_atoms, not atom's atomNum()'s.
    // atomNum()'s arent unique, there can be several atoms
    // in molecule which represent same atomNum

    for(int idx=0; idx<m_atoms.count(); ++idx)
    {
        // we'll write pos through using QPoint
        // I'd use QPair but it isn't supported by QVariant
        QPoint pos(m_atoms.at(idx)->fieldX(), m_atoms.at(idx)->fieldY());
        config.writeEntry( QString("Atom_%1").arg(idx), pos);
    }

    // save undo history
    int moveCount = m_undoStack.count();
    config.writeEntry( "MoveCount", moveCount );
    AtomMove mv;
    for(int i=0;i<moveCount;++i)
    {
        mv = m_undoStack.at(i);
        // atomIdx, direction, numCells
        QList<int> move;
        move << mv.atomIdx << static_cast<int>(mv.dir) << mv.numCells;
        config.writeEntry( QString("Move_%1").arg(i), move );
    }
    config.writeEntry("SelectedAtom", m_selIdx);
    config.writeEntry("LevelFinished", m_levelFinished );
}

void PlayField::loadGame( const KConfigGroup& config )
{
    // it is assumed that this method is called right after loadLevel() so
    // level itself is already loaded at this point

    // read atom positions
    for(int idx=0; idx<m_atoms.count(); ++idx)
    {
        QPoint pos = config.readEntry( QString("Atom_%1").arg(idx), QPoint() );
        m_atoms.at(idx)->setFieldXY(pos.x(), pos.y());
        m_atoms.at(idx)->setPos( toPixX(pos.x()), toPixY(pos.y()) );
    }
    // fill undo history
    m_numMoves = config.readEntry("MoveCount", 0);

    AtomMove mv;
    for(int i=0;i<m_numMoves;++i)
    {
        QList<int> move = config.readEntry( QString("Move_%1").arg(i), QList<int>() );
        mv.atomIdx = move.at(0);
        mv.dir = static_cast<Direction>(move.at(1));
        mv.numCells = move.at(2);
        m_undoStack.push(mv);
    }
    if(m_numMoves)
    {
        emit enableUndo(true);
        emit updateMoves(m_numMoves);
    }

    m_selIdx = config.readEntry("SelectedAtom", 0);
    m_levelFinished = config.readEntry("LevelFinished", false);
    updateArrows();
}

void PlayField::saveLastBackground()
{
    KAtomicRenderer::self()->saveBackground(sceneRect().size().toSize());
}

void PlayField::showMessage( const QString& message )
{
    m_messageItem->setMessageTimeout( 4000 );
    m_messageItem->showMessage( message, KGamePopupItem::BottomLeft );
}

#include "playfield.moc"
