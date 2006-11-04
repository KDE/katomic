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
#ifndef PLAYFIELD_H
#define PLAYFIELD_H
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QList>

#define FIELD_SIZE 15

class KSimpleConfig;
class KAtomicRenderer;
class QResizeEvent;
class Molecule;
class FieldGraphicsItem;
class QTimeLine;

/**
 *  KAtomic level playfield
 */
class PlayField : public QGraphicsScene
{
    Q_OBJECT
public:
    enum Direction { Up, Down, Left, Right };
    /**
     *  Constructor
     */
    explicit PlayField( QObject *parent );
    /**
     *  Resizes playfield to width,height
     */
    void resize( int width, int height );
    /**
     *  Loads level from config file
     */
    void loadLevel(const KSimpleConfig& config);
    /**
     *  Sets current molecule
     */
    void setMolecule(Molecule *mol) { m_mol = mol; }
    /**
     *  Selects next atom
     */
    void nextAtom();
    /**
     *  Selects previous atom
     */
    void previousAtom();
    /**
     *  Animates currently selected atom movement in direction dir
     */
    void moveSelectedAtom( Direction dir );
private slots:
    void animFrameChanged(int frame);
signals:
    void gameOver(int numMoves);

private:
    virtual void drawBackground( QPainter*, const QRectF& );
    virtual void mousePressEvent( QGraphicsSceneMouseEvent* ev );
    /**
     *  Checks if molecule is finished
     */
    bool checkDone() const;
    /**
     *  Re-renders atoms&arrows Pixmaps, updates their positions
     */
    void updateFieldItems();
    /**
     *  Updates arrows around selected atom
     */
    void updateArrows(bool justHide=false);
    /**
     *  Returns true if Field cell (x,y) is empty, i.e. it isn't a wall and has no atom
     */
    bool cellIsEmpty(int x, int y) const;
    /**
     *  Molecule to be done
     */
    Molecule *m_mol;
    /**
     *  Number of moves made for current level
     */
    int m_numMoves;
    /**
     *  Represents level.
     *  True means there's a wall, false means no wall
     */
    bool m_field[FIELD_SIZE][FIELD_SIZE];
    /**
     *  Element (i.e. atom, wall, arrow) size
     */
    int m_elemSize;
    /**
     *  Renderer which is used to render elements from svg
     */
    KAtomicRenderer *m_renderer;
    /**
     *  List of atom QGraphicsItems
     */
    QList<FieldGraphicsItem*> m_atoms;
    /**
     *  Arrow items
     */
    FieldGraphicsItem *m_upArrow, *m_leftArrow, *m_downArrow, *m_rightArrow;
    /**
     *  Currently selected atom
     */
    FieldGraphicsItem *m_selAtom;

    /**
     *  Direction in which current atom animation moves
     */
    Direction m_dir;
    /**
     *  Timeline object to control animation
     */
    QTimeLine *m_timeLine;
};

class PlayFieldView : public QGraphicsView
{
public:
    PlayFieldView( PlayField* field, QWidget *parent );
private:
    void resizeEvent(QResizeEvent *);

    PlayField *m_playField;
};

#endif
