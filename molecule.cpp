/*******************************************************************
 *
 * Copyright (C) Andreas Wüst <AndreasWuest@gmx.de>
 * Copyright (C) Stephan Kulow <coolo@kde.org>
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
#include <ksimpleconfig.h>
#include <klocale.h>
#include <kdebug.h>

#include <QPainter>
#include <QPaintEvent>

#include "molecule.h"
#include "katomicrenderer.h"

extern int level;

Molecule::Molecule( QWidget *parent ) 
   : QWidget( parent )
{
    QPalette palette;
    palette.setColor( backgroundRole(), Qt::black );
    setPalette(palette);
    setAutoFillBackground( true );
    setMinimumSize(240, 200);

    m_elemSize = 20;

    m_renderer = new KAtomicRenderer( KStandardDirs::locate("appdata", "pics/default_theme.svgz"), this );
    m_renderer->setElementSize( m_elemSize );
}

Molecule::~Molecule ()
{
}

const atom& Molecule::getAtom(int index) const
{
    static atom none = { 0, "" };

    if (index > atoms.count() || index == 0)
        return none;

    return atoms.at(index - 1);
}

void Molecule::load (const KSimpleConfig& config)
{
    atoms.clear();
    QString key;

    atom current;

    int atom_index = 1;
    QString value;
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
    }

    QString line;

    for (int j = 0; j < MOLECULE_SIZE; j++) {

        key.sprintf("mole_%d", j);
        line = config.readEntry(key,QString());

        for (int i = 0; i < MOLECULE_SIZE; i++)
        {
            if (i >= line.size()) molek[i][j] = 0;
            else molek[i][j] = atom2int(line.at(i).toLatin1());
        }
    }

    mname = i18n(config.readEntry("Name", I18N_NOOP("Noname")).toLatin1());

    update();
}

void Molecule::paintEvent( QPaintEvent * )
{
    QString  st = i18n("Level: %1", level);

    QPainter  painter(this);
    painter.setPen (QColor (190, 190, 190));
    painter.drawText (7, height() - 36, mname);
    painter.drawText (7, height() - 18, st);

    // Paint the playing field 
    for (int i = 0; i < MOLECULE_SIZE; i++)
        for (int j = 0; j < MOLECULE_SIZE; j++) {
            int x = 10 + i * m_elemSize;
            int y = 10 + j * m_elemSize;

            if (molek[i][j] == 0)
                continue;

            QPixmap aPix = m_renderer->renderAtom(getAtom(molek[i][j]));
            painter.drawPixmap(x, y, aPix);
        }

    painter.end ();
}

#include "molecule.moc"
