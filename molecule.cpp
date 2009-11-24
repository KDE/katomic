/*******************************************************************
 *
 * Copyright (C) Andreas Wuest <AndreasWuest@gmx.de>
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

#include "molecule.h"

const atom& Molecule::getAtom(int index) const
{
    static atom none = { 0, "" };

    if (index > m_atoms.count() || index <= 0)
        return none;

    return m_atoms.at(index - 1);
}

