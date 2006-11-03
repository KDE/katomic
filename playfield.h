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
#ifndef PLAYFIELD_H
#define PLAYFIELD_H
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QList>

#define FIELD_SIZE 15

class KSimpleConfig;
class KAtomicRenderer;
class QResizeEvent;
class Molek;
class AtomGraphicsItem;

class PlayField : public QGraphicsScene
{
public:
    PlayField( QObject *parent );
    void resize( int width, int height );

    void load (const KSimpleConfig& config);
    void setMolecule(Molek *mol) { m_mol = mol; }
private:
    virtual void drawBackground( QPainter*, const QRectF& );
    void renderAtoms();

    Molek *m_mol;
    char m_field[FIELD_SIZE][FIELD_SIZE];
    int m_elemSize;
    KAtomicRenderer *m_renderer;

    QList<AtomGraphicsItem*> m_atoms;
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
