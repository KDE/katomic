/*
    SPDX-FileCopyrightText: 2006-2009 Dmitry Suzdalev <dimsuz@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KATOMIC_COMMONDEFS_H
#define KATOMIC_COMMONDEFS_H

#define FIELD_SIZE 15

#define DEFAULT_LEVELSET_NAME "default_levels"

namespace KAtomic
{
    enum LevelSetRoles
    {
        LevelSetGuiNameRole = Qt::DisplayRole,
        LevelSetNameRole = Qt::UserRole,
        LevelSetAuthorRole = Qt::UserRole+1,
        LevelSetAuthorEmailRole = Qt::UserRole+2,
        LevelSetDescriptionRole = Qt::UserRole+3,
        LevelSetLevelCountRole = Qt::UserRole+4
    };
} // namespace KAtomic

#endif
