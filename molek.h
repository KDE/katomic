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


class Molek : public QWidget
{
    Q_OBJECT

public:
    Molek (QWidget *parent=0, const char *name=0);
   ~Molek ();

public slots:

    void loadFeldFromDat (int);

   
signals: 



protected:
    void paintEvent( QPaintEvent * );

private:
  
  QPixmap data;
  
  struct spielfeld
  {
    unsigned char obj;
    unsigned int verb;
  } molek [10] [10];

  // name of molekul
  char mname [30];

  int level;
};

#endif // MOLEK_H
