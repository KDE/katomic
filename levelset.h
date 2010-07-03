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
#ifndef KATOMIC_LEVELSET_H
#define KATOMIC_LEVELSET_H

#include <QString>
#include <QList>

#include <KSharedConfig>

#include "commondefs.h"

class Molecule;

/**
 * Class that represents a KAtomic level
 */
class LevelData
{
public:
    ~LevelData();

    struct Element
    {
        int atom; // == -1 for walls
        int x;
        int y;

        Element() : atom(-1), x(-1), y(-1) {}
    };

    QList<Element> atomElements() const;

    bool containsWallAt(int x, int y) const;

    /**
     * A pointer to molecule object that is the target of this level
     */
    const Molecule* molecule() const;

private:
    friend class LevelSet;

    // @param elements contain atoms and walls. for walls 'atom' field will be -1
    // @param molecule - molecule to be solved. LevelData takes ownership of this object and will
    // delete it
    LevelData(const QList<Element>& elements, const Molecule* mol);
    LevelData(const LevelData&);

    QList<Element> m_atoms;
    bool m_field[FIELD_SIZE][FIELD_SIZE];

    const Molecule* m_molecule;
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
    bool loadFromFile(const QString& fileName);

    const LevelData* levelData(int levelNum) const;

    /**
     * Returns name of the levelset. In general this name shouldn't be used in gui.
     * To get the name suitable to showing in gui @see visibleName
     */
    QString name() const;

    /**
     * @return name of the levelset which is suitable for showing in gui
     */
    QString visibleName() const;

    /**
     * @return name of the author of the levelset
     */
    QString author() const;

    /**
     * @return email of the author of the levelset
     */
    QString authorEmail() const;

    /**
     * @return description of the levelset
     */
    QString description() const;

    /**
     * @return number of levels in this levelset
     */
    int levelCount() const;

    /**
     * Checks if default level set is installed on disk
     */
    static bool isDefaultLevelsAvailable();

private:
    void reset();
    const LevelData* readLevel(int levelNum) const;
    const Molecule* readLevelMolecule(int levelNum) const;

private:
    KSharedConfigPtr m_levelsFile;
    mutable QHash<int, LevelData*> m_levelCache;

    QString m_name;
    QString m_visibleName;
    QString m_description;
    QString m_author;
    QString m_authorEmail;
    int m_levelCount;
};

#endif
