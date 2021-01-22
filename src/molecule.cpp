/*
    This file is part of the KDE project "KAtomic"

    SPDX-FileCopyrightText: Andreas Wuest <AndreasWuest@gmx.de>
    SPDX-FileCopyrightText: Stephan Kulow <coolo@kde.org>
    SPDX-FileCopyrightText: 2006 Dmitry Suzdalev <dimsuz@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "molecule.h"

const atom& Molecule::getAtom(int index) const
{
    static atom none = { 0, "" };

    if (index > m_atoms.count() || index <= 0)
        return none;

    return m_atoms.at(index - 1);
}

