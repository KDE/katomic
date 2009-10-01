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

#include <KStandardDirs>
#include <KConfigGroup>
#include <KDebug>

#include "atom.h"

LevelData::LevelData(const QList<Element>& elements)
{
    memset(m_field, 0, sizeof(m_field));
    foreach (const Element& el, elements)
    {
        if (el.atom == -1)
        {
            m_field[el.x][el.y] = true; // a wall
        }
        else
        {
            m_atoms.append(el);
        }
    }
    kDebug() << "creating level data: atoms =" << m_atoms.count();
}

LevelData::~LevelData()
{

}

QList<LevelData::Element> LevelData::atomElements() const
{
    return m_atoms;
}

bool LevelData::containsWallAt(int x, int y) const
{
    return m_field[x][y] == true;
}

const Molecule* LevelData::molecule() const
{
    return 0;
}

// ==================================================

LevelSet::LevelSet()
{
    reset();
}

LevelSet::~LevelSet()
{
    qDeleteAll(m_levelCache);
}

void LevelSet::reset()
{
    m_visibleName = QString();
    m_description = QString();
    m_author = QString();
    m_authorEmail = QString();

    qDeleteAll(m_levelCache);
    m_levelCache.clear();
}

bool LevelSet::load(const QString& levelSetName)
{
    reset();

    QString file = KStandardDirs::locate("appdata", QString("levels/%1.dat").arg(levelSetName));
    if (file.isEmpty())
    {
        kDebug() << "level set \"" << levelSetName << "\" data file not found. Check your installation";
        return false;
    }

    m_levelsFile = KSharedConfig::openConfig( file, KConfig::SimpleConfig);
    KConfigGroup gr = m_levelsFile->group("LevelSet");

    m_visibleName = gr.readEntry("Name");
    m_description = gr.readEntry("Description");
    m_author = gr.readEntry("Author");
    m_authorEmail = gr.readEntry("AuthorEmail");

    return true;
}

const LevelData* LevelSet::levelData(int levelNum) const
{
    LevelData* data = m_levelCache.value(levelNum, 0);
    return data ? data : readLevel(levelNum);
}

const LevelData* LevelSet::readLevel(int levelNum) const
{
    KConfigGroup config = m_levelsFile->group("Level"+QString::number(levelNum));
    QString key;

    QList<LevelData::Element> elements;

    for (int j = 0; j < FIELD_SIZE; j++)
    {
        key.sprintf("feld_%02d", j);
        QString line = config.readEntry(key,QString());

        for (int i = 0; i < FIELD_SIZE; i++)
        {
            QChar c = line.at(i);
            if( c == '#' )
            {
                LevelData::Element el;
                el.x = i;
                el.y = j;
                el.atom = -1; // indicates wall

                elements.append(el);
            }
            else if (c != '.')//atom
            {
                LevelData::Element el;
                el.x = i;
                el.y = j;
                el.atom = atom2int(c.toLatin1());

                elements.append(el);
            }
        }
    }

    LevelData* level = new LevelData(elements);
    m_levelCache[levelNum] = level;

    return level;
}
