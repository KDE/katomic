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
#ifndef KATOMIC_RENDERER_H
#define KATOMIC_RENDERER_H

#include <QPixmap>
#include <QHash>

class atom;
class KSvgRenderer;
class KPixmapCache;

/**
 *  Class for rendering elements of game SVG to QPixmap
 */
class KAtomicRenderer
{
public:
    static KAtomicRenderer* self();

    /**
     *  Renders atom to pixmap.
     *  If setElementSize() wasn't called it assumes size of 30 px
     *  Atom pixmaps are cached
     */
    QPixmap renderAtom( const atom&, int size ) const;
    /**
     *  Renders non-atom elements (wall and arrows) to pixmap.
     *  @param element if == '#' will render a wall, if '<','>','^','_' will render
     *  arrow-left,arrow-right,arrow-up or arrow-down
     *  Elements pixmaps are cached
     */
    QPixmap renderNonAtom( char element, int size ) const;
    /**
     *  Renders backgound to QPixmap of size set by setBackgroundSize
     *  Background pixmap is cached
     */
    QPixmap renderBackground(const QSize& size) const;
private:
    KAtomicRenderer();
    ~KAtomicRenderer();

    // disable copy - it's singleton
    KAtomicRenderer( const KAtomicRenderer& );
    KAtomicRenderer& operator=( const KAtomicRenderer& );

    /**
     * Creates hashes for translating atom and bond signatures found in
     * level files to corresponding SVG-element names
     */
    void fillNameHashes();

    KSvgRenderer *m_renderer;
    QHash<char, QString> m_names; // cryptic_char -> elemName
    QHash<char, QString> m_bondNames; // cryptic_char -> bondName
    KPixmapCache *m_cache;
};

#endif
