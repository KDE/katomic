/* highscore.cpp
 *
 * Andreas Wüst
 *
 */

#include "highscore.h"
#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>

Highscore::Highscore ( QWidget *parent, const char *name, int l, int moves)
    : QDialog ( parent, name, TRUE )
{
  level = l;
  QString tmp;
  const int width = 270;
  const int height = 260;
  const int anzahl = 10;

  setFixedSize(width, height); 
  setCaption(i18n("Highscore"));

  loadHighscore ();

  pos = 1000;

  debug ("-------------------\nlevel : %d", level);
  debug (" in highscore moves : %d" , moves);
  if (moves > 0)
  {
    // er mitteln, an welcher position man sich mit den punkten befindet !  
    //    pos = 1000;
    int i = 5;
    while (moves <= score [i].moves && i >= 0)
    {
      pos = i;
      i--;
    }

    debug ("pos %d", pos + 1);

    // nachfolgende positionen verschieben
    i = 5;
    while (i > pos && i > 0)
    {
      fprintf (stderr, "in neupos\n");
      score [i].name = score [i - 1].name;
      score [i].moves = score [i - 1].moves;
      i--;
    }
    if (pos != 1000)
      score [pos].moves = moves; 
  }
  
 
  
  fprintf (stderr, "nach neupositionierung\n");
 
  n = new QLabel *[anzahl];

  for (int i = 0; i < 6; i++)
  {
    // position + name ausgeben
    tmp.sprintf("%2d.   ", i + 1);
    tmp += score [i].name;
    n [i] = new QLabel (tmp, this);
    n [i]->setGeometry (30, 20 + i * 30, 140, 25);       
    
    // punkte ausgeben
    tmp.sprintf ("%4d", score [i].moves);
    n [i] = new QLabel (tmp, this);
    n [i]->setGeometry (190, 20 + i * 30, 40, 25);     
  }

  // pushbutton erzeugen             
  ok = new QPushButton(i18n("OK"), this, "ok");
  connect(ok, SIGNAL(clicked()), SLOT(accept()) );
  ok->setGeometry(width / 2 - 30, height - 40 , 60, 30);

  
  fprintf (stderr, "vor line edit\n ");

  // line edit erzeugen
  debug ("pos %d", pos);

  if (pos < 6 && moves > 0)
  {
    fprintf (stderr, "line edit erzeugen");
    le = new QLineEdit (this, "le");
    le->setFixedHeight (le->sizeHint ().height ());
    le->setGeometry (50, 20 + pos * 30, 130, 25);
    le->setMaxLength (18);
    connect (le, SIGNAL (returnPressed ()), 
             this, SLOT (eingabeFertig ()));

    // wenn position in highscore liste erreicht, dann ok button verstecken
    ok->hide ();

  } 
  
  fprintf (stderr, "ende consturctor\n");
}

void Highscore::getChangedText (const QString& s)
{
  fprintf (stderr, "%seingabe fertig\n", s.ascii()); 
}

void Highscore::eingabeFertig ()
{
  fprintf (stderr, "\nfinished\n");

  // eingegebener text in s 

  QString s = le->text ();
  score [pos].name = s;
  saveHighscore();

  delete le;
  le = 0;

  // s als label ausgeben
  QLabel *l = new QLabel (s, this);
  l->setGeometry (56, 20 + pos * 30 , 130, 25);
  l->show ();
 
  // pushbutton ok anzeigen
  ok->show ();

}


void Highscore::loadHighscore ()
{
    KConfig *config = KGlobal::config();
    QString group = QString("level%1").arg(level);
    KConfigGroupSaver cs(config, group);
    QString key;
    for (int i = 0; i < 6; i++) {
	key.sprintf("Name%d", i);
	score[i].name = config->readEntry(key, i18n("Joe Noname"));
	key.sprintf("Moves%d", i);
	score[i].moves = config->readNumEntry(key, (6-i) * 100);
	debug("read %s %d", score[i].name.ascii(), score[i].moves);
    }
}

void Highscore::saveHighscore ()
{
    KConfig *config = KGlobal::config();
    QString group = QString("level%1").arg(level);
    KConfigGroupSaver cs(config, group);
    QString key;
    for (int i = 0; i < 6; i++) {
	key.sprintf("Name%d", i);
	config->writeEntry(key, score[i].name);
	key.sprintf("Moves%d", i);
	config->writeEntry(key, score[i].moves);
    }
    config->sync();
}


Highscore::~Highscore()
{
  fprintf (stderr, "in destructor\n");
  delete [] n;
}

#include "highscore.moc" 
