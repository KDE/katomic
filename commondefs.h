/*
  Copyright (C) 2006-2009   Dmitry Suzdalev (dimsuz@gmail.com)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
  */
#ifndef _COMMONDEFS_H
#define _COMMONDEFS_H

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
