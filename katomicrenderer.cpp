/*******************************************************************
 *
 * Copyright 2006-2007 Dmitry Suzdalev <dimsuz@gmail.com>
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
#include "katomicrenderer.h"
#include "atom.h"
#include "prefs.h"

#include <KSvgRenderer>
#include <KDebug>
#include <KStandardDirs>

#include <kpixmapcache.h>
#include <QPainter>

KAtomicRenderer* KAtomicRenderer::self()
{
    static KAtomicRenderer instance;
    return &instance;
}

KAtomicRenderer::KAtomicRenderer()
{
    m_cache = new KPixmapCache("katomic-cache");
    m_cache->setCacheLimit( 3*1024 );

    m_renderer = new KSvgRenderer( KStandardDirs::locate( "appdata", "pics/default_theme.svgz" ) );
    fillNameHashes();
}

KAtomicRenderer::~KAtomicRenderer()
{
    delete m_cache;
    delete m_renderer;
}

void KAtomicRenderer::fillNameHashes()
{
    m_names['1'] = "atom_H";
    m_names['2'] = "atom_C";
    m_names['3'] = "atom_O";
    m_names['4'] = "atom_N";
    m_names['5'] = "atom_S";
    m_names['6'] = "atom_F";
    m_names['7'] = "atom_Cl";
    m_names['8'] = "atom_Br";
    m_names['9'] = "atom_P";
    m_names['0'] = "atom_J";
    m_names['o'] = "atom_Crystal";
    m_names['A'] = "connector_Hor";
    m_names['B'] = "connector_Slash";
    m_names['C'] = "connector_Ver";
    m_names['D'] = "connector_Backslash";
    m_names['#'] = "wall";
    m_names['<'] = "arrow_Left";
    m_names['>'] = "arrow_Right";
    m_names['^'] = "arrow_Up";
    m_names['_'] = "arrow_Down";
    m_names['E'] = "atom_flask0";
    m_names['F'] = "atom_flask1";
    m_names['G'] = "atom_flask2";
    m_names['H'] = "atom_flask3";
    m_names['I'] = "atom_flask4";
    m_names['J'] = "atom_flask5";
    m_names['K'] = "atom_flask6";
    m_names['L'] = "atom_flask7";

    m_bondNames['a'] = "bond_I_Top";
    m_bondNames['b'] = "bond_I_TopRight";
    m_bondNames['c'] = "bond_I_Right";
    m_bondNames['d'] = "bond_I_BotRight";
    m_bondNames['e'] = "bond_I_Bottom";
    m_bondNames['f'] = "bond_I_BotLeft";
    m_bondNames['g'] = "bond_I_Left";
    m_bondNames['h'] = "bond_I_TopLeft";

    m_bondNames['A'] = "bond_II_Top";
    m_bondNames['B'] = "bond_II_Right";
    m_bondNames['C'] = "bond_II_Bottom";
    m_bondNames['D'] = "bond_II_Left";

    m_bondNames['E'] = "bond_III_Top";
    m_bondNames['F'] = "bond_III_Right";
    m_bondNames['G'] = "bond_III_Bottom";
    m_bondNames['H'] = "bond_III_Left";
}

QPixmap KAtomicRenderer::renderAtom( const atom& at, int size ) const
{
    if (!m_renderer->isValid() || size == 0) return QPixmap();

    QString cacheName = QString("atom_%1_%2").arg(at.obj).arg(size);
    QPixmap atomPix;
    if(!m_cache->find(cacheName, atomPix))
    {
        kDebug() << "putting" << cacheName << "to cache";
        atomPix = QPixmap(size,size);
        atomPix.fill(Qt::transparent);
        QPainter p(&atomPix);
        m_renderer->render(&p, m_names.value(at.obj) );
        p.end();
        m_cache->insert(cacheName, atomPix);
    }

    QPainter p;
    QPixmap bonds(size,size);
    bonds.fill(Qt::transparent);
    for (int c = 0; c < MAX_CONNS_PER_ATOM; c++)
    {
        char conn = at.conn[c];
        if (!conn)
            break;
        cacheName = QString("bond_%1_%2").arg(conn).arg(size);
        QPixmap pix;
        if(!m_cache->find(cacheName, pix))
        {
            kDebug() << "putting" << cacheName << "to cache";
            pix = QPixmap(size,size);
            pix.fill(Qt::transparent);
            p.begin(&pix);
            m_renderer->render(&p, m_bondNames.value(conn) );
            p.end();
            m_cache->insert(cacheName, pix);
        }
        p.begin(&bonds);
        p.drawPixmap(0,0, pix);
        p.end();
    }

    p.begin(&bonds);
    p.drawPixmap(0,0, atomPix);
    p.end();
    return bonds;
}

QPixmap KAtomicRenderer::renderNonAtom( char element, int size ) const
{
    if (!m_renderer->isValid() || size == 0) return QPixmap();

    QString cacheName = QString("nonAtom_%1_%2").arg(element).arg(size);
    QPixmap pix;
    if(!m_cache->find(cacheName,pix))
    {
        kDebug() << "putting" << cacheName << "to cache";
        pix = QPixmap(size,size);
        pix.fill(Qt::transparent);
        QPainter p(&pix);
        m_renderer->render(&p, m_names.value(element) );
        p.end();
        m_cache->insert(cacheName, pix);
    }

    return pix;
}

QPixmap KAtomicRenderer::renderBackground(const QSize& size) const
{
    QPixmap pix;
    QString cacheName = QString("background_%1x%2").arg(size.width()).arg(size.height());
    if(!m_cache->find(cacheName, pix))
    {
        kDebug() << "putting" << cacheName << "to cache";
        pix = QPixmap(size);
        pix.fill(Qt::transparent);
        QPainter p(&pix);
        m_renderer->render(&p, "background");
        p.end();
        m_cache->insert(cacheName, pix);
    }
    return pix;
}
