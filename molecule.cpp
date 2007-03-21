/*******************************************************************
 *
 * Copyright (C) Andreas Wüst <AndreasWuest@gmx.de>
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
// bemerkungen : wenn paintEvent aufgerufen wird, wird das komplette
//               widget gelöscht und nur die sachen gezeichnet, die in
//               paintEvent stehen ! sollen dinge z.b nur bei maustasten-
//               druck gezeichnet werden, so muß dies in mousePressEvent
//               stehen !
//               paintEvent wird aufgerufen, falls fenster überdeckt wird,
//               oder auch einfach bewegt wird

#include <kdefakes.h>
#include <kstandarddirs.h>
#include <kconfig.h>
#include <klocale.h>
#include <kdebug.h>

#include <QPainter>
#include <QPaintEvent>

#include "molecule.h"
#include "katomicrenderer.h"

extern int level;

const atom& Molecule::getAtom(int index) const
{
    static atom none = { 0, "" };

    if (index > atoms.count() || index == 0)
        return none;

    return atoms.at(index - 1);
}

void Molecule::load (const KConfig& config)
{
    atoms.clear();
    QString key;

    atom current;

    int atom_index = 1;
    QString value;
    m_weight = 0.0;
    while (true) {
        key.sprintf("atom_%c", int2atom(atom_index));
        value = config.readEntry(key,QString());
        if (value.isEmpty())
            break;

        current.obj = value.at(0).toLatin1();
        value = value.mid(2);
        if (value.isNull())
            value = "";

        strlcpy(current.conn, value.toAscii(), sizeof(current.conn));
        kWarning( atoms.indexOf(current) != -1 )
            << "OOOPS, duplicate atom definition in " << key << endl;
        atoms.append(current);
        atom_index++;
	m_weight += current.weight();
    }

    QString line;

    m_width = 0;
    m_height = 0;

    int max_i = -1;
    for (int j = 0; j < MOLECULE_SIZE; j++) {

        key.sprintf("mole_%d", j);
        line = config.readEntry(key,QString());

        int max_non_null_i = -1;
        for (int i = 0; i < MOLECULE_SIZE; i++)
        {
            if (i >= line.size())
                molek[i][j] = 0;
            else 
            {
                molek[i][j] = atom2int(line.at(i).toLatin1());
                max_non_null_i = i;
            }
        }
        if( max_non_null_i != -1 )
            m_height++;
        max_i = qMax( max_i, max_non_null_i );
    }

    m_width = max_i+1;

    mname = i18n(config.readEntry("Name", I18N_NOOP("Noname")).toLatin1());
}

MoleculeRenderer::MoleculeRenderer()
    : m_mol(0)
{
    m_renderer = new KAtomicRenderer( KStandardDirs::locate("appdata", "pics/default_theme.svgz") );
    setAtomSize(20);
}

MoleculeRenderer::~MoleculeRenderer ()
{
    delete m_renderer;
}

void MoleculeRenderer::setAtomSize( int size )
{
    m_atomSize = size;
    m_renderer->setElementSize( size );
}

void MoleculeRenderer::render( QPainter *painter, const QPoint& o) const
{
    /* 
    QString   st = i18n("Level: %1", level);

    // Paint the name and level of the molecule at the bottom of the widget.
    painter->setPen (QColor (190, 190, 190));
    painter->drawText (7, height() - 36, mname);
    painter->drawText (7, height() - 18, st);
     */

    int originX = o.x() - m_atomSize*m_mol->width()/2;
    int originY = o.y() - m_atomSize*m_mol->height()/2;

    // Paint the playing field 
    for (int i = 0; i < MOLECULE_SIZE; i++)
        for (int j = 0; j < MOLECULE_SIZE; j++)
        {
            int x = originX + i * m_atomSize;
            int y = originY + j * m_atomSize;

            if (m_mol->getAtom(i,j) == 0)
                continue;

            int atomIdx = m_mol->getAtom(i,j);
            QPixmap aPix = m_renderer->renderAtom(m_mol->getAtom(atomIdx));
            painter->drawPixmap(x, y, aPix);
        }
}
