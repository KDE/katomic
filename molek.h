/****************************************************************
**
** Definition of Molek class,
**
****************************************************************/

#ifndef MOLEK_H
#define MOLEK_H

#include <stdio.h>
#include <stdlib.h>
#include <qwidget.h>
#include <qpoint.h>
#include <qpainter.h>
#include <qpixmap.h>
#include "atom.h"
#include <qvaluelist.h>

class KSimpleConfig;

#define MOLEK_SIZE 15

class Molek : public QWidget
{
    Q_OBJECT

public:
    Molek (QWidget *parent=0, const char *name=0);
   ~Molek ();

   void load(const KSimpleConfig& config);

   const atom& getAtom(uint index) const;
   int atomSize() const { return atoms.count(); }

   QSize molecSize() const { return _size; }
   uint getAtom(int x, int y) const { return molek[x][y]; }
   
protected:
   void paintEvent( QPaintEvent * );

private:
   QPixmap data;
   uint molek[MOLEK_SIZE][MOLEK_SIZE]; // the indexes within atoms
   QValueList<atom> atoms;
   QString mname;
   QSize _size;


};

#endif // MOLEK_H
