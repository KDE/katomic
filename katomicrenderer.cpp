/*******************************************************************
 *
 * Copyright 2006 Dmitry Suzdalev <dimsuz@gmail.com>
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

#include <KSvgRenderer>
#include <KDebug>
#include <KStandardDirs>

#include <QPixmap>
#include <QPixmapCache>
#include <QPainter>

KAtomicRenderer* KAtomicRenderer::self()
{
    static KAtomicRenderer instance;
    return &instance;
}

KAtomicRenderer::KAtomicRenderer()
{
    QPixmapCache::setCacheLimit( 5000 ); // around 5 Mb
    m_renderer = new KSvgRenderer( KStandardDirs::locate( "appdata", "pics/default_theme.svgz" ) );
    fillNameHashes();
}

KAtomicRenderer::~KAtomicRenderer()
{
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
    if (!m_renderer->isValid()) return QPixmap();

    ensureAtomIsInCache(at, size);

    QString cacheName;
    QPixmap res(size, size);
    res.fill( Qt::transparent );
    QPainter p(&res);
    QPixmap bond_pix;
    // paint connections first
    for (int c = 0; c < MAX_CONNS_PER_ATOM; c++)
    {
        char conn = at.conn[c];
        if (!conn)
            break;
        cacheName = QString("bond_%1_%2").arg(conn).arg(size);
        if(QPixmapCache::find(cacheName, bond_pix))
            p.drawPixmap( 0, 0, bond_pix );
    }
    // and now the atom
    QPixmap atom_pix;
    cacheName = QString("atom_%1_%2").arg(at.obj).arg(size);
    if(QPixmapCache::find(cacheName, atom_pix))
        p.drawPixmap( 0, 0, atom_pix );
    p.end();
    return res;
}

QPixmap KAtomicRenderer::renderNonAtom( char element, int size ) const
{
    if (!m_renderer->isValid()) return QPixmap();

    QString cacheName = QString("nonAtom_%1_%2").arg(element).arg(size);
    QPixmap pix;
    if(!QPixmapCache::find(cacheName,pix))
    {
        QImage baseImg(size, size, QImage::Format_ARGB32_Premultiplied);
        //Fill the buffer, it is unitialised by default
        baseImg.fill(0);
        QPainter p(&baseImg);
        m_renderer->render(&p, m_names.value(element), QRectF(0,0, size, size) );
        pix = QPixmap::fromImage(baseImg);
        QPixmapCache::insert(cacheName, pix);
    }

    return pix;
}

QPixmap KAtomicRenderer::renderBackground(const QSize& size) const
{
//    kDebug() << "renderBackground" << size << endl;
    QString cacheName = QString("bkgnd_%1_%2").arg(size.width()).arg(size.height());
    QPixmap bkgnd;
    if( !QPixmapCache::find(cacheName, bkgnd)  )
    {
        bkgnd = QPixmap(size);
        QPainter p(&bkgnd);
        m_renderer->render(&p, "background");

        QPixmapCache::insert( cacheName, bkgnd );
    }
    return bkgnd;
}

void KAtomicRenderer::ensureAtomIsInCache(const atom& at, int size) const
{
    QImage baseImg;
    QString cacheName = QString("atom_%1_%2").arg(at.obj).arg(size);
    if(!QPixmapCache::find(cacheName))
    {
        //Construct an image object to render the contents of the .svgz file
        baseImg = QImage(size, size, QImage::Format_ARGB32_Premultiplied);
        //Fill the buffer, it is unitialised by default
        baseImg.fill(0);
        QPainter p(&baseImg);
        m_renderer->render(&p, m_names.value(at.obj), QRectF(0,0, size, size) );
        QPixmap atomPix = QPixmap::fromImage(baseImg);
        QPixmapCache::insert(cacheName, atomPix);
    }

    for (int c = 0; c < MAX_CONNS_PER_ATOM; c++)
    {
        char conn = at.conn[c];
        if (!conn)
            break;
        cacheName = QString("bond_%1_%2").arg(conn).arg(size);
        if(!QPixmapCache::find(cacheName))
        {
            //Construct an image object to render the contents of the .svgz file
            baseImg = QImage(size, size, QImage::Format_ARGB32_Premultiplied);
            //Fill the buffer, it is unitialised by default
            baseImg.fill(0);
            QPainter p(&baseImg);
            m_renderer->render(&p, m_bondNames.value(conn), QRectF(0,0, size, size) );
            QPixmap bondPix = QPixmap::fromImage(baseImg);
            QPixmapCache::insert(cacheName, bondPix);
        }
    }
}

void KAtomicRenderer::saveBackground(const QSize& size) const
{
    QPixmap bkgnd = renderBackground(size);
    bkgnd.save( KStandardDirs::locateLocal( "appdata", "savedBkgnd.png" ) );
}

void KAtomicRenderer::restoreSavedBackground()
{
    // put last saved background (if any) to cache immediately :-)
    QString fname = KStandardDirs::locate( "appdata", "savedBkgnd.png" );
    if ( !fname.isEmpty() )
    {
        QPixmap pix( fname );
        QString cacheName = QString("bkgnd_%1_%2").arg(pix.width()).arg(pix.height());
        kDebug() << "inserting " << cacheName << endl;
        QPixmapCache::insert( cacheName, pix );
    }
    else
        kDebug() << "no last saved pixmap found" << endl;

}
