/*******************************************************************
 *
 * Copyright 2006-2009 Dmitry Suzdalev <dimsuz@gmail.com>
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
#ifndef _LEVELSET_H
#define _LEVELSET_H

#include <QString>

class Molecule;

/**
 * Class that represents a KAtomic level
 */
class LevelData
{
public:
    ~LevelData();

    enum ElementType { AtomElement, WallElement, EmptyElement };

    /**
     * @return type of the element at cell with [x,y] coords
     */
    ElementType elementTypeAt(int x, int y) const;

    /**
     * @return atom id of atom at [x,y], and -1 if no atom there
     */
    int atomAt(int x, int y) const;

    /**
     * A pointer to molecule object that is the target of this level
     */
    const Molecule* molecule() const;

private:
    friend class LevelSet;
    LevelData();
    LevelData(const LevelData&);
};

/**
 * Represents a set of levels. Implements loading of level set given its name
 */
class LevelSet
{
public:
    LevelSet();
    ~LevelSet();

    bool load(const QString& levelSetName);
    const LevelData* levelData(int levelNum) const;
};

#endif
