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
#include <stdio.h>




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
        KLined *le;
   

        
public slots:
        void getChangedText (const char *);
        void eingabeFertig ();
        void loadHighscore ();
        void saveHighscore ();
     
        

private:

   QPushButton *ok;
   struct highscore
   {
     char name [20];
     int moves;
   } score [6];
   int pos;
   int level;

   protected :
     void makeNewHighscore ();

};

#endif		
