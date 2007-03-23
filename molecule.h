/*******************************************************************
 *
 * Copyright (C) Andreas Wüst <AndreasWuest@gmx.de>
 * Copyright (C) Stephan Kulow <coolo@kde.org>
 * Copyright (C) 2006 Dmitry Suzdalev <dimsuz@gmail.com>
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
#ifndef MOLECULE_H
#define MOLECULE_H

#include <QWidget>
#include <QPixmap>
#include <QList>
#include "atom.h"

class KConfig;
class KAtomicRenderer;

#define MOLECULE_SIZE 15

/**
  this class represents one molecule
*/
class Molecule
{
public:
    Molecule() : m_width(0), m_height(0) { };

    void load(const KConfigGroup& config);

    const atom& getAtom(int index) const;

    uint getAtom(int x, int y) const { return molek[x][y]; }

    /**
     *  Width of molecule measured in atoms
     */
    int width() const { return m_width; }
    /**
     *  Height of molecule measured in atoms
     */
    int height() const { return m_height; }

    /**
      * @return the name of the molecule
      */
    QString moleculeName() const { return mname; }

    /**
    * @return the molecule weight of the molecule
    */
    double molecularWeight() const { return m_weight; }

private:
    uint molek[MOLECULE_SIZE][MOLECULE_SIZE]; // the indexes within atoms
    QList<atom> atoms;
    QString mname;

    ///the molecule weight of the Molecule
    double m_weight;

    int m_width;
    int m_height;
};

/**
 *  Uses KAtomicRenderer to render molecules
 */
class MoleculeRenderer
{
public:
    MoleculeRenderer();
    ~MoleculeRenderer();

    void setMolecule( const Molecule* mol ) { m_mol = mol; }
    void setAtomSize(int size);
    /**
     *  Renders molecule using painter.
     *  Molecule will be centered around a point specified in parameter 'o'
     */
    void render( QPainter *painter, const QPoint& o ) const;

    const Molecule* const molecule() const { return m_mol; }
private:
   KAtomicRenderer* m_renderer;
   int m_atomSize;
   const Molecule* m_mol;
};
#endif // MOLECULE_H
