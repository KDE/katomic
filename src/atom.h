/*
    This file is part of the KDE project "KAtomic"

    SPDX-FileCopyrightText: 1999 Stephan Kulow <coolo@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef ATOM_H
#define ATOM_H

#define MAX_CONNS_PER_ATOM 8

class atom {
public:
    char obj;
    char conn[MAX_CONNS_PER_ATOM + 1];

    bool operator==(atom rhs) const { return (rhs.obj == obj && !strcmp(rhs.conn,conn)); }
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
