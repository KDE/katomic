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
#include <qmessagebox.h>

#include <kscoredialog.h>

#include "atom.h"

class KSimpleConfig;
class Molek;

#define FIELD_SIZE 15

class Feld : public QWidget
{
    Q_OBJECT

public:
    Feld (QWidget *parent=0, const char *name=0);
    ~Feld ();

    enum Direction { None, MoveUp, MoveDown, MoveLeft, MoveRight };

    void startAnimation (Direction dir);
    void done ();

    void load (const KSimpleConfig& config);

    void setMolek(Molek *_mol) { mol = _mol; }

signals:
    void gameOver(int moves);
    void sendMoves(int moves);

protected:
    bool checkDone();
    void timerEvent (QTimerEvent *);
    void paintEvent( QPaintEvent * );
    void paintMovingAtom();
    void mousePressEvent (QMouseEvent *);
    void mouseMoveEvent (QMouseEvent *);
    void emitStatus();

public slots:
	void nextAtom();
        void previousAtom();

private:

    const atom& getAtom(uint index) const;

    void putNonAtom(int, int, Direction, bool brick = false);

    QPoint *point;
    QPixmap data;
    QPixmap sprite;

    Molek *mol;

    uint feld[FIELD_SIZE][FIELD_SIZE];

    // number of movements
    int moves;

    Direction dir;
    int cx, cy;
    int xpos, ypos;
    int anz;
    int frames, framesbak;

    bool anim;
    bool chosen, moving;

    void resetValidDirs();

};

#endif // FELD_H
