/*******************************************************************
 *
 * Copyright 2006 Dmitry Suzdalev <dimsuz@gmail.com>
 *
 * This file is part of the KDE project "KReversi"
 *
 * KReversi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * KReversi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with KReversi; see the file COPYING.  If not, write to
 * the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 ********************************************************************/
#include <ksvgrenderer.h>
#include <kdebug.h>
#include <QPixmap>
#include <QPainter>

#include "atomrenderer.h"
#include "atom.h"

KAtomicRenderer::KAtomicRenderer( const QString& pathToSvg, QObject *parent )
{
    m_renderer = new KSvgRenderer( pathToSvg, parent);
    fillNameHashes();
    setAtomSize(30);
}

void KAtomicRenderer::setAtomSize( int size )
{
    m_atomSize = size;
    // TODO re-render cache contents. Or clear?
}

QPixmap KAtomicRenderer::renderAtom( const atom& at )
{
    if (!m_renderer->isValid()) return QPixmap();
    // FIXME dimsuz: move this to function. smth like ensureInCache(at)
    QImage baseImg;
    if(!m_atomCache.contains(at.obj))
    {
        kDebug() << "putting atom to cache" << endl;
        //Construct an image object to render the contents of the .svgz file
        baseImg = QImage(m_atomSize, m_atomSize, QImage::Format_ARGB32_Premultiplied);
        //Fill the buffer, it is unitialised by default
        baseImg.fill(0);
        QPainter p(&baseImg);
        m_renderer->render(&p, m_atomNames.value(at.obj), QRectF(0,0, m_atomSize, m_atomSize) );
        QPixmap atomPix = QPixmap::fromImage(baseImg);
        m_atomCache[at.obj] = atomPix;
    }
    else
        kDebug() << "reusing atom from cache" << endl;

    for (int c = 0; c < MAX_CONNS_PER_ATOM; c++)
    {
        char conn = at.conn[c];
        if (!conn)
            break;
        if(!m_bondCache.contains(conn))
        {
            kDebug() << "putting bond to cache" << endl;
            //Construct an image object to render the contents of the .svgz file
            baseImg = QImage(m_atomSize, m_atomSize, QImage::Format_ARGB32_Premultiplied);
            //Fill the buffer, it is unitialised by default
            baseImg.fill(0);
            QPainter p(&baseImg);
            m_renderer->render(&p, m_bondNames.value(conn), QRectF(0,0, m_atomSize, m_atomSize) );
            QPixmap bondPix = QPixmap::fromImage(baseImg);
            m_bondCache[conn] = bondPix;
        }
        else
            kDebug() << "reusing bond from cache" << endl;
    }

    QPixmap res(m_atomSize, m_atomSize);
    res.fill( Qt::transparent );
    QPainter p(&res);
    // paint connections first
    for (int c = 0; c < MAX_CONNS_PER_ATOM; c++)
    {
        char conn = at.conn[c];
        if (!conn)
            break;
        p.drawPixmap( 0, 0, m_bondCache.value( conn ) );
    }
    // and now the atom
    p.drawPixmap( 0, 0, m_atomCache.value( at.obj ) );
    p.end();
    return res;
}

void KAtomicRenderer::fillNameHashes()
{
    m_atomNames['1'] = "atom_H";
    m_atomNames['2'] = "atom_C";
    m_atomNames['3'] = "atom_O";
    m_atomNames['4'] = "atom_N";
    m_atomNames['5'] = "atom_S";
    m_atomNames['6'] = "atom_F";
    m_atomNames['7'] = "atom_Cl";
    m_atomNames['8'] = "atom_Br";
    m_atomNames['9'] = "atom_P";
    m_atomNames['0'] = "atom_J";
    m_atomNames['o'] = "atom_Crystal";
    m_atomNames['A'] = "connector_Hor";
    m_atomNames['B'] = "connector_Slash";
    m_atomNames['C'] = "connector_Ver";
    m_atomNames['D'] = "connector_Backslash";

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
