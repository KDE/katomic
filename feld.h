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
#include "atom.h"

class KSimpleConfig;
class Molek;

class Feld : public QWidget
{
    Q_OBJECT

public:
    Feld (Molek *mol, QWidget *parent=0, const char *name=0);
    ~Feld ();
    void startAnimation (int dir);
    void done ();

    void load (const KSimpleConfig& config);

signals: 
    void showDir ();
    void hideDir ();
    void gameOver(int moves);

protected:
    bool checkDone ();    
    void timerEvent (QTimerEvent *);
    void paintEvent( QPaintEvent * );
    void mousePressEvent (QMouseEvent *);
    void mouseReleaseEvent (QMouseEvent *);  
    void mouseMoveEvent (QMouseEvent *);
    
private:

    const atom& getAtom(int index) const; 

    Highscore *high;
    
    QPoint *point;
    QPixmap data;
    QPixmap sprite;

    Molek *mol;

    int feld[15][15];
    
    // number of movements
    int moves; 
    
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
