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
#ifndef ATOM_RENDERER_H
#define ATOM_RENDERER_H

#include <QHash>

class atom;
class KSvgRenderer;

/**
 *  Class for rendering atoms to QPixmap
 */
class AtomRenderer
{
public:
    /**
     *  Constructor.
     *  @param pathToSvg path to svg containing atom, bonds etc pictures
     */
    AtomRenderer( const QString& pathToSvg, QObject *parent=0 );
    /**
     *  Sets atom size
     */
    void setAtomSize(int size);
    /**
     *  Renders atom to pixmap.
     *  If setAtomSize() wasn't called it assumes size of 30 px
     */
    QPixmap renderAtom( const atom& );
private:
    /**
     * FIXME dimsuz: describe better
     *  Fills hashes atom[bond]_name->SVG_element_name
     */
    void fillNameHashes();
    KSvgRenderer *m_renderer;
    // I'd use a two hashes (for Cache and for names)
    // instead of four, but currently both bond and atom chars can be
    // "A-D", i.e. they overlap, so I need to separate
    // If level format changes somehow, then corresponding atom and bond hashes 
    // can be merged
    // FIXME dimsuz: document them
    QHash<char, QPixmap> m_atomCache;
    QHash<char, QPixmap> m_bondCache;
    QHash<char, QString> m_atomNames; // cryptic_char -> elemName
    QHash<char, QString> m_bondNames; // cryptic_char -> bondName
    int m_atomSize;
};

#endif
