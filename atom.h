/*******************************************************************
 *
 * Copyright (C) 1999 Stephan Kulow <coolo@kde.org>
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
#ifndef ATOM_H
#define ATOM_H

#define MAX_CONNS_PER_ATOM 8

class atom {
public:
    char obj;
    char conn[MAX_CONNS_PER_ATOM + 1];

    bool operator==(const atom& rhs) const { return (rhs.obj == obj && !strcmp(rhs.conn,conn)); }
    bool isEmpty() const { return (obj == 0 || obj == '.'); }
    double weight() const {
        switch (obj) {
            case '1':	return   1.00797;	// H
            case '2':	return  12.0107;	// C
            case '3':	return  15.9994;	// O
            case '4':	return  14.0067;	// N
            case '5':	return  32.065;		// S
            case '6':	return  18.9984;	// Fl
            case '7':	return  35.453;		// Cl
            case '9':	return  30.97;		// P
            default:	return   0.0;
        }
    }
};

inline char int2atom(int i) { 
    if (!i)
        return '.';
    if (i == 254)
        return '#';
    if (i <= 9) 
        return i + '0';
    return i + 'a' - 10;
}

inline int atom2int(char ch) {
    if (ch == '.' || ch == 0)
        return 0;
    if (ch == '#')
        return 254;
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    return ch - 'a' + 10;
}

#endif
