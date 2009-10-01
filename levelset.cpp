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
#include "levelset.h"

LevelData::LevelData()
{

}

LevelData::~LevelData()
{

}

LevelData::ElementType LevelData::elementTypeAt(int x, int y) const
{
    return EmptyElement;
}

int LevelData::atomAt(int x, int y) const
{
    return -1;
}

const Molecule* LevelData::molecule() const
{
    return 0;
}

// ==================================================

LevelSet::LevelSet()
{

}

LevelSet::~LevelSet()
{

}

bool LevelSet::load(const QString& levelSetName)
{
    return false;
}

const LevelData* LevelSet::levelData(int levelNum) const
{
    return 0;
}
