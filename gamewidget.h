
#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

class Feld;
class Molek;
class QScrollBar;
class QLabel;

#include <qwidget.h>

class GameWidget : public QWidget
{
    Q_OBJECT

 public:
	
    GameWidget ( QWidget *parent, const char *name=0 );

    ~GameWidget();
		
 protected:

    // stellt das spielfeld dar !
    Feld *feld;

    // stellt molek�l dar
    Molek *molek;

    // scorllbar zur levelwahl
    QScrollBar *scrl;

    // important labels : highest and current scores
    QLabel *hs, *ys;
    QString highest, current;
	
    int nlevels;

 public slots:
    // bringt level auf neuesten stand
    void updateLevel (int);

    // getbutton erh�lt button der gedr�ckt wurde
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

};

#endif
