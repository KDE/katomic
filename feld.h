/****************************************************************
**
** Definition of Feld class,
**
****************************************************************/

#ifndef FELD_H
#define FELD_H

#include <QWidget>
#include <QPixmap>

#include "atom.h"

#define MAX_UNDO (100)

class KSimpleConfig;
class Molek;

#define FIELD_SIZE 15

class Feld : public QWidget
{
    Q_OBJECT

public:
    Feld (QWidget *parent=0);
    ~Feld ();

    enum Direction { None      = 0,
                     MoveUp    = 1,
                     MoveDown  = (-1),
                     MoveLeft  = 2,
                     MoveRight = (-2) };

    void startAnimation (Direction dir);
    void done ();

    void load (const KSimpleConfig& config);

    void setMolek(Molek *_mol) { mol = _mol; }

    void doUndo ();
    void doRedo ();

signals:
    void gameOver(int moves);
    void sendMoves(int moves);
    void enableRedo(bool enable);
    void enableUndo(bool enable);

protected:
    bool checkDone();
    void timerEvent (QTimerEvent *);
    void paintEvent( QPaintEvent * );
    void paintMovingAtom(QPainter &paint);
    void mousePressEvent (QMouseEvent *);
    void mouseMoveEvent (QMouseEvent *);
    void emitStatus();

protected:
    struct UndoInfo {
      uint atom;
      int  oldxpos, oldypos;
      int  xpos, ypos;
      Direction dir;
    };

public slots:
	void nextAtom();
        void previousAtom();

private:

    const atom& getAtom(uint index) const;

    void putNonAtom(int, int, Direction, QPainter &p, bool brick = false);

    QPoint *point;
    QPixmap data;
    QPixmap sprite;
    QPixmap copy;

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

    uint undoBegin;
    uint undoSize;
    uint redoSize;
    UndoInfo undo[MAX_UNDO];

    void resetValidDirs();

};

#endif // FELD_H
