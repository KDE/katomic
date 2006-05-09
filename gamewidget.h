//Added by qt3to4:
#include <QLabel>

#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

class Feld;
class Molek;
class QScrollBar;
class QLabel;
class KScoreDialog;

#include <QWidget>

class GameWidget : public QWidget
{
    Q_OBJECT

 public:

    GameWidget ( QWidget *parent );

    ~GameWidget();

 signals:
    void enableRedo(bool enable);
    void enableUndo(bool enable);

 public slots:
    // bringt level auf neuesten stand
    void updateLevel (int);

    // restart current level
    void restartLevel();

    // getbutton erhält button der gedrückt wurde
    void getButton (int);

    void gameOver(int moves);

    // use this slot to update the moves continually
    void getMoves(int moves);

    // Menupunkt Highscores im Pop-up Menu, der Highscore anzeigt
    void showHighscores ();

    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
    void nextAtom();
    void previousAtom();
    void doUndo ();
    void doRedo ();

 protected:

    // stellt das spielfeld dar !
    Feld *feld;

    // stellt molekül dar
    Molek *molek;

    // scorllbar zur levelwahl
    QScrollBar *scrl;

    // important labels : highest and current scores
    QLabel *hs, *ys;
    QString highest, current;

    int nlevels;

    KScoreDialog *highScore;
};

#endif
