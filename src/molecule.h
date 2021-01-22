/*
    This file is part of the KDE project "KAtomic"

    SPDX-FileCopyrightText: Andreas Wüst <AndreasWuest@gmx.de>
    SPDX-FileCopyrightText: Stephan Kulow <coolo@kde.org>
    SPDX-FileCopyrightText: 2006-2007 Dmitry Suzdalev <dimsuz@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef MOLECULE_H
#define MOLECULE_H

#include "atom.h"

#include <QList>
#include <QString>

class KConfig;

#define MOLECULE_SIZE 15

/**
 this class represents one molecule
 */
class Molecule
{
public:
    const atom& getAtom(int index) const;

    uint getAtom(int x, int y) const { return m_molek[x][y]; }

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
    QString moleculeName() const { return m_name; }

    /**
     * @return the molecule weight of the molecule
     */
    double molecularWeight() const { return m_weight; }

private:
    friend class LevelSet;

    Molecule() : m_width(0), m_height(0), m_weight(0) { }

    uint m_molek[MOLECULE_SIZE][MOLECULE_SIZE]; // the indexes within atoms
    QList<atom> m_atoms;
    QString m_name;

    int m_width;
    int m_height;

    ///the molecule weight of the Molecule
    double m_weight;

};

#endif // MOLECULE_H
