/* highscore.h
 *
 * Andreas Wüst
 *
 */

#ifndef HIGHSCORE_H
#define HIGHSCORE_H

#include <kapp.h>
#include <klined.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qpushbt.h>




/**
 * This a dialog the configure the colors used by MyWidget
 * It's an extension to QWidget and isn't very difficult to understand.
 *
 */

class Highscore : public QDialog
{

    Q_OBJECT

	public:
    Highscore ( QWidget *, const char *name, int level, int moves);
    ~Highscore();

    // positionen + namen
    QLabel **n;	
    QLineEdit *le;
   
 public slots:
    void getChangedText (const QString&);
    void eingabeFertig ();
    void loadHighscore ();
    void saveHighscore ();
     
 private:

    QPushButton *ok;
    struct highscore
    {
	QString name;
	int moves;
    } score [6];

    int pos;
    int level;

};

#endif		
