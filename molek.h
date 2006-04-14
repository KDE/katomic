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
//Added by qt3to4:
#include <QPaintEvent>
#include "atom.h"
#include <q3valuelist.h>
#include <QList>

class KSimpleConfig;

#define MOLEK_SIZE 15

class Molek : public QWidget
{
    Q_OBJECT

public:
    Molek (QWidget *parent=0);
   ~Molek ();

   void load(const KSimpleConfig& config);

   const atom& getAtom(int index) const;
   int atomSize() const { return atoms.count(); }

   QSize molecSize() const { return _size; }
   uint getAtom(int x, int y) const { return molek[x][y]; }
   
protected:
   void paintEvent( QPaintEvent * );

private:
   QPixmap data;
   uint molek[MOLEK_SIZE][MOLEK_SIZE]; // the indexes within atoms
   QList<atom> atoms;
   QString mname;
   QSize _size;


};

#endif // MOLEK_H
