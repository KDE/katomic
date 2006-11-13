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

class KSimpleConfig;
class KAtomicRenderer;

#define MOLECULE_SIZE 15

class Molecule
{
public:
    Molecule() { };

    void load(const KSimpleConfig& config);

    const atom& getAtom(int index) const;

    uint getAtom(int x, int y) const { return molek[x][y]; }
private:
    uint molek[MOLECULE_SIZE][MOLECULE_SIZE]; // the indexes within atoms
    QList<atom> atoms;
    QString mname;
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
    void render( QPainter *painter ) const;
private:
   KAtomicRenderer* m_renderer;
   int m_atomSize;
   const Molecule* m_mol;
};
#endif // MOLECULE_H
