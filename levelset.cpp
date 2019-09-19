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


#include <KConfigGroup>
#include <QDebug>
#include <KLocalizedString>
#include <QFileInfo>

#include <string.h>
#include <QStandardPaths>
#include <KSharedConfig>

#include "atom.h"
#include "molecule.h"
#include "commondefs.h"

LevelData::LevelData(const QList<Element>& elements, const Molecule* mol)
    : m_molecule(mol)
{
    memset(m_field, 0, sizeof(m_field));
    for (const Element& el : elements)
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
}

LevelData::~LevelData()
{
    delete m_molecule;
}

QList<LevelData::Element> LevelData::atomElements() const
{
    return m_atoms;
}

bool LevelData::containsWallAt(int x, int y) const
{
    return m_field[x][y];
}

const Molecule* LevelData::molecule() const
{
    return m_molecule;
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
    m_name = QString();
    m_visibleName = QString();
    m_description = QString();
    m_author = QString();
    m_authorEmail = QString();
    m_levelCount = 0;

    qDeleteAll(m_levelCache);
    m_levelCache.clear();
}

bool LevelSet::load(const QString& levelSetName)
{
    QString file = QStandardPaths::locate(QStandardPaths::AppDataLocation, QStringLiteral("levels/%1.dat").arg(levelSetName));
    if (file.isEmpty())
    {
        qDebug() << "level set \"" << levelSetName << "\" data file not found. Check your installation";
        return false;
    }

    bool res = loadFromFile(file);
    if (!res) {
        qDebug() << "warning: failed to load level set" << levelSetName;
    }

    return res;
}

bool LevelSet::loadFromFile(const QString& fileName)
{
    reset();

    m_levelsFile = KSharedConfig::openConfig(fileName, KConfig::SimpleConfig);
    KConfigGroup gr = m_levelsFile->group("LevelSet");

    m_visibleName = gr.readEntry("Name");
    m_description = gr.readEntry("Description");
    m_author = gr.readEntry("Author");
    m_authorEmail = gr.readEntry("AuthorEmail");
    m_levelCount = gr.readEntry("LevelCount", 0);

    m_name = QFileInfo(fileName).baseName();

    if (m_levelCount <= 0) {
        //qDebug() << "warning: in level set" << m_name << "level count not specified or invalid";
    }

    return true;
}

QString LevelSet::name() const
{
    return m_name;
}

QString LevelSet::visibleName() const
{
    return m_visibleName;
}

QString LevelSet::author() const
{
    return m_author;
}

QString LevelSet::authorEmail() const
{
    return m_authorEmail;
}

QString LevelSet::description() const
{
    return m_description;
}

int LevelSet::levelCount() const
{
    return m_levelCount;
}

const LevelData* LevelSet::levelData(int levelNum) const
{
    LevelData* data = m_levelCache.value(levelNum, nullptr);
    return data ? data : readLevel(levelNum);
}

const LevelData* LevelSet::readLevel(int levelNum) const
{
    KConfigGroup config = m_levelsFile->group(QStringLiteral("Level")+QString::number(levelNum));

    QList<LevelData::Element> elements;

    for (int j = 0; j < FIELD_SIZE; j++)
    {
        const QString key = QString::asprintf("feld_%02d", j);
        QString line = config.readEntry(key,QString());

        for (int i = 0; i < FIELD_SIZE; i++)
        {
            if (line.isEmpty())
            {
                //qDebug() << "error while reading level" << levelNum << "data from" << m_name;
                return nullptr;
            }

            QChar c = line.at(i);
            if( c == QLatin1Char('#') )
            {
                LevelData::Element el;
                el.x = i;
                el.y = j;
                el.atom = -1; // indicates wall

                elements.append(el);
            }
            else if (c != QLatin1Char('.'))//atom
            {
                LevelData::Element el;
                el.x = i;
                el.y = j;
                el.atom = atom2int(c.toLatin1());

                elements.append(el);
            }
        }
    }

    // Molecule object will be deleted by LevelData, it takes ownership
    LevelData* level = new LevelData(elements, readLevelMolecule(levelNum));
    m_levelCache[levelNum] = level;

    return level;
}

const Molecule* LevelSet::readLevelMolecule(int levelNum) const
{
    Molecule* mol = new Molecule();
    KConfigGroup config = m_levelsFile->group(QStringLiteral("Level")+QString::number(levelNum));


    atom current;

    int atom_index = 1;
    QString value;
    while (true) {
        const QString key = QString::asprintf("atom_%c", int2atom(atom_index));
        value = config.readEntry(key,QString());
        if (value.isEmpty())
            break;

        current.obj = value.at(0).toLatin1();
        value = value.mid(2);

        strncpy(current.conn, value.toLatin1().constData(), sizeof(current.conn));
        if (mol->m_atoms.indexOf(current) != -1)
            qWarning()
                << "OOOPS, duplicate atom definition in" << key;
        mol->m_atoms.append(current);
        atom_index++;
    }

    QString line;

    mol->m_width = 0;
    mol->m_height = 0;
    mol->m_weight = 0.0;

    int max_i = -1;
    for (int j = 0; j < MOLECULE_SIZE; j++) {

        const QString key = QString::asprintf("mole_%d", j);
        line = config.readEntry(key,QString());

        int max_non_null_i = -1;
        for (int i = 0; i < MOLECULE_SIZE; i++)
        {
            if (i >= line.size())
                mol->m_molek[i][j] = 0;
            else
            {
                mol->m_molek[i][j] = atom2int(line.at(i).toLatin1());
                mol->m_weight += mol->getAtom(mol->m_molek[i][j]).weight();
                max_non_null_i = i;
            }
        }
        if( max_non_null_i != -1 )
            mol->m_height++;
        max_i = qMax( max_i, max_non_null_i );
    }

    mol->m_width = max_i+1;

    mol->m_name = i18n(config.readEntry("Name", I18N_NOOP("Noname")).toUtf8().constData());

    return mol;
}

bool LevelSet::isDefaultLevelsAvailable()
{
    QString file = QStandardPaths::locate(QStandardPaths::AppDataLocation, QStringLiteral("levels/%1.dat").arg(QLatin1String(DEFAULT_LEVELSET_NAME)));
    if (file.isEmpty())
    {
        //qDebug() << "default level set \"" << DEFAULT_LEVELSET_NAME << "\" data file not found. Check your installation";
        return false;
    }

    return true;
}
