/* highscore.cpp
 *
 * Andreas Wüst
 *
 */

#include "highscore.moc" 
#include <klocale.h>

Highscore::Highscore ( QWidget *parent, const char *name, int l, int moves)
                : QDialog ( parent, name, TRUE )
{
  int i;

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
    i = 5;
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
      memcpy (&score [i], &score [i - 1], sizeof (score [i - 1]));
      score [i].moves = score [i - 1].moves;
      i--;
    }
    if (pos != 1000) 
      score [pos].moves = moves; 
  }
  
 
  
  fprintf (stderr, "nach neupositionierung\n");

 
  n = new QLabel *[anzahl];

  for (i = 0; i < 6; i++)
  {
    // position + name ausgeben
    tmp.sprintf ("%2d.   %s", i + 1, score [i].name);
    n [i] = new QLabel (tmp.data (), this);
    n [i]->setGeometry (30, 20 + i * 30, 140, 25);       
    
    // punkte ausgeben
    tmp.sprintf ("%4d", score [i].moves);
    n [i] = new QLabel (tmp.data (), this);
    n [i]->setGeometry (190, 20 + i * 30, 40, 25);     
  }

  // pushbutton erzeugen             
  ok = new QPushButton(i18n("OK"), this, "ok");
  connect(ok, SIGNAL(clicked()), this, SLOT(accept()) );
  ok->setGeometry(width / 2 - 30, height - 40 , 60, 30);

  
  fprintf (stderr, "vor line edit\n ");

  // line edit erzeugen
  debug ("pos %d", pos);

  if (pos < 6 && moves > 0)
  {
    fprintf (stderr, "line edit erzeugen");
    le = new KLineEdit (this, "le");
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

void Highscore::getChangedText (const char *s)
{
  fprintf (stderr, "%seingabe fertig\n", s); 
}

void Highscore::eingabeFertig ()
{
  fprintf (stderr, "\nfinished\n");

  // eingegebener text in s 

  QString s = le->text ();
  // alten string löschen, neuen eintragen
  memset (&score [pos].name, 0, sizeof (score [pos].name));
  memcpy (&score [pos].name, s, strlen (s));

  delete le;

  // s als label ausgeben
  QLabel *l = new QLabel (s.data (), this);
  l->setGeometry (56, 20 + pos * 30 , 130, 25);
  l->show ();
 
  // pushbutton ok anzeigen
  ok->show ();

}


void Highscore::loadHighscore ()
{
  debug ("in loadhighscore");
  FILE *in;

  if ( (in = fopen ("highscore.dat", "rb")) == NULL)
  {
    makeNewHighscore ();
  }
  else
  { 
    fseek (in, (level - 1) * sizeof (score), SEEK_SET);
    fread (&score [0], sizeof (score), 1, in);
  }
  fclose (in);
}

void Highscore::makeNewHighscore ()
{
  debug ("in makenewhighscore");
  FILE *out;
  if ( (out = fopen ("highscore.dat", "wb")) != NULL)
  {
   debug ("datei nicht gefunden");

    char st [9] = {"John Doe"};

    for (int j = 0; j < 67; j++)
    {
 
      for (int i = 0; i < 6; i++)
      {
        score [i].moves = 500 + i * 100;
        memcpy (&score [i].name, &st, sizeof (st));
        debug ("%s", score [i].name);
      }

      // datei neu anlegen
      debug ("--------------------d anlegen");
  
      fwrite (score, sizeof (score), 1, out);

    }
  }
  fclose (out);
}


void Highscore::saveHighscore ()
{
  FILE *out;
  
  if ( (out = fopen ("highscore.dat", "r+b")) == NULL)
  {
    debug ("konnte datei nicht schreiben");
  }
  else
  {
    fseek (out, (level - 1) * sizeof (score), SEEK_SET);
    fwrite (score, sizeof (score), 1, out);
  }

  fclose (out);
  
}


Highscore::~Highscore()
{
  fprintf (stderr, "in destructor\n");
  saveHighscore ();
  delete [] n; 
  delete ok;        
}
