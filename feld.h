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
#define FIELD_SIZE 15

class KSimpleConfig;
class Molecule;
class KAtomicRenderer;

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

    void setMolecule(Molecule *_mol) { mol = _mol; }

    void doUndo ();
    void doRedo ();
public slots:
    void nextAtom();
    void previousAtom();

signals:
    void gameOver(int moves);
    void sendMoves(int moves);
    void enableRedo(bool enable);
    void enableUndo(bool enable);

private:
    void timerEvent (QTimerEvent *);
    void paintEvent( QPaintEvent * );
    void resizeEvent( QResizeEvent * );
    void mousePressEvent (QMouseEvent *);
    void mouseMoveEvent (QMouseEvent *);

    bool checkDone();
    void paintMovingAtom(QPainter &paint);
    void emitStatus();

    struct UndoInfo {
      uint atom;
      int  oldxpos, oldypos;
      int  xpos, ypos;
      Direction dir;
    };

    const atom& getAtom(uint index) const;

    void resetValidDirs();

    QPixmap sprite;
    QPixmap copy;

    Molecule *mol;

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

    KAtomicRenderer *m_renderer;
    int m_elemSize;
};

#endif // FELD_H
