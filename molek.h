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
#include <atom.h>

class KSimpleConfig;

class Molek : public QWidget
{
    Q_OBJECT

public:
    Molek (QWidget *parent=0, const char *name=0);
   ~Molek ();

   void load(const KSimpleConfig& config);

   const atom& getAtom(int index) const { return *atoms.at(index); }
   int atomSize() const { return atoms.count(); }

protected:
   void paintEvent( QPaintEvent * );

private:
  
   QPixmap data;
   int molek [10] [10]; // the indexes within atoms
   QValueList<atom> atoms;
   QString mname;

};

#endif // MOLEK_H
