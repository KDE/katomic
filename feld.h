/****************************************************************
**
** Definition of Feld class,
**
****************************************************************/

#ifndef FELD_H
#define FELD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <qwidget.h>
#include <qpoint.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qmsgbox.h>

#include "highscore.h"


class Feld : public QWidget
{
    Q_OBJECT

public:
    Feld (QWidget *parent=0, const char *name=0);
    ~Feld ();
    void startAnimation (int dir);
    void done ();

public slots:

    void loadFeldFromDat (int);

   
signals: 
  void showDir ();
  void hideDir ();


protected:
  bool checkDone ();    
  void timerEvent (QTimerEvent *);
  void paintEvent( QPaintEvent * );
  void mousePressEvent (QMouseEvent *);
  void mouseReleaseEvent (QMouseEvent *);  
  void mouseMoveEvent (QMouseEvent *);
  


private:
  
  Highscore *high;

  
  QPoint *point;
  QPixmap data;
  QPixmap sprite;

  struct spielfeld
  {
    unsigned char obj;
    unsigned int verb;
  } molek [10] [10], feld [15] [15];

  // number of movements
  int moves; 


  // Höhe und breite des Mokelüls
  int hohe, breite;
  int level;
  int dir;
  int cx, cy;
  int xpos, ypos;
  int anz;
  int frames, framesbak;
  int speed;

  bool anim;
  bool pressed, chosen, moving;


};

#endif // FELD_H
