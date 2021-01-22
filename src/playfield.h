/*
    This file is part of the KDE project "KAtomic"

    SPDX-FileCopyrightText: 2006-2007 Dmitry Suzdalev <dimsuz@gmail.com>
    SPDX-FileCopyrightText: 2010 Brian Croom <brian.s.croom@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef PLAYFIELD_H
#define PLAYFIELD_H

#include "commondefs.h"

#include <KGameRenderer>

#include <QGraphicsScene>
#include <QList>
#include <QStack>

#define MIN_ELEM_SIZE 30

class KConfigGroup;
class AtomFieldItem;
class ArrowFieldItem;
class MoleculePreviewItem;
class QTimeLine;
class KGamePopupItem;
class LevelData;

/**
 *  KAtomic level playfield
 */
class PlayField : public QGraphicsScene
{
    Q_OBJECT
public:
    enum Direction { Up=0, Down, Left, Right };
    explicit PlayField( QObject *parent );
    ~PlayField();
    /**
     *  Resizes playfield to width,height
     */
    void resize( int width, int height );
    /**
     *  Loads level
     */
    void setLevelData(const LevelData* level);
    /**
     *  Sets animation speed (0-slow, 1-normal, 2-fast)
     */
    void setAnimationSpeed(int speed);
    /**
     *  Animates currently selected atom movement in direction dir
     *  @param numCells used on undos/redos
     */
    void moveSelectedAtom( Direction dir, int numCells=0 );
    /**
     *  Saves the current game to config object
     */
    void saveGame(KConfigGroup& config) const;
    /**
     *  Loads game from config object
     */
    void loadGame(const KConfigGroup& config);
    /**
     *  Returns whether level is finished already
     */
    bool isLevelFinished() const { return m_levelFinished; }
    /**
     * Displays a passive popup message at the bottom of the scene
     */
    void showMessage( const QString& message );
    /**
     * Name of the current molecule
     */
    QString moleculeName() const;
    /**
     * The caching SVG pixmap renderer
     */
    KGameRenderer* renderer() { return &m_renderer; }

public Q_SLOTS:
    /**
     *  Selects next atom
     */
    void nextAtom();
    /**
     *  Selects previous atom
     */
    void previousAtom();
    /**
     *  Undoes one movement
     */
    void undo();
    /**
     *  Redoes one movement
     */
    void redo();
    /**
     *  Undoes all movements
     */
    void undoAll();
    /**
     *  Redoes all movements
     */
    void redoAll();
Q_SIGNALS:
    void gameOver(int numMoves);
    void updateMoves(int);
    void enableUndo(bool);
    void enableRedo(bool);
private Q_SLOTS:
    void atomAnimFrameChanged(int frame);
private:
    void drawForeground( QPainter*, const QRectF& ) override;
    void mousePressEvent( QGraphicsSceneMouseEvent* ev ) override;

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
     * Set the background brush to a properly sized pixmap
     */
    void updateBackground();
    /**
     *  Returns true if Field cell (x,y) is empty, i.e. it isn't a wall and has no atom
     */
    bool cellIsEmpty(int x, int y) const;
    /**
     *  Returns true if atom animation is running
     */
    bool isAnimating() const;

    inline int toPixX( int fieldX ) const { return fieldX*m_elemSize; }
    inline int toPixY( int fieldY ) const { return fieldY*m_elemSize; }
    inline int toFieldX( int pixX ) const { return pixX/m_elemSize; }
    inline int toFieldY( int pixY ) const { return pixY/m_elemSize; }
    inline int fieldCenterX() const { return toPixX(0) + m_elemSize*FIELD_SIZE/2; }
    inline int fieldCenterY() const { return toPixY(0) + m_elemSize*FIELD_SIZE/2; }

    /**
     * Renderer object
     */
    KGameRenderer m_renderer;
    /**
     *  Number of moves made for current level
     */
    int m_numMoves;
    /**
     * Level Data
     */
    const LevelData* m_levelData;
    /**
     *  Element (i.e. atom, wall, arrow) size
     */
    int m_elemSize;
    /**
     *  List of atom QGraphicsItems
     */
    QList<AtomFieldItem*> m_atoms;
    /**
     *  Arrow items
     */
    ArrowFieldItem *m_upArrow, *m_leftArrow, *m_downArrow, *m_rightArrow;
    /**
     * Item used to show messages to user
     */
    KGamePopupItem *m_messageItem;
    /**
     *  Index of currently selected atom
     */
    int m_selIdx;
    /**
     *  Direction in which current atom animation moves
     */
    Direction m_dir;
    /**
     *  Animation speed. Atom will move at 1cell in m_animSpeed msec speed
     */
    int m_animSpeed;
    /**
     *  Timeline object to control atom movement animation
     */
    QTimeLine *m_atomTimeLine;
    /**
     *  True if current level is finished and thus all player input should be disabled
     */
    bool m_levelFinished;

    struct AtomMove
    {
        int atomIdx; // atom index in m_atoms
        Direction dir;
        int numCells;
        AtomMove( int idx=-1, Direction d=Up, int nc=0 )
            : atomIdx(idx), dir(d), numCells(nc) { }
    };
    QStack<AtomMove> m_undoStack;
    QStack<AtomMove> m_redoStack;

    MoleculePreviewItem *m_previewItem;
};

#endif
