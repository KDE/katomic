/* highscore.cpp
 *
 * Andreas Wüst
 *
 */

#include "highscore.h"
#include <qlayout.h>
#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>

Highscore::Highscore ( QWidget *parent, const char *name, int l, int moves)
    : QDialog ( parent, name, TRUE )
{
  level = l;
  QString tmp;

  setCaption(i18n("Highscore"));

  loadHighscore ();

  pos = 1000;

  debug ("-------------------\nlevel : %d", level);
  debug (" in highscore moves : %d" , moves);

  if (moves > 0)
  {
    int i = 5;
    while (moves <= score [i].moves && i >= 0)
    {
      // this is the position where the new score goes
      pos = i;
      i--;
    }

    debug ("pos %d", pos + 1);

    // nachfolgende positionen verschieben
    i = 5;
    while (i > pos && i > 0)
    {
      debug ( "in neupos\n");
      score [i].name = score [i - 1].name;
      score [i].moves = score [i - 1].moves;
      i--;
    }
    if (pos != 1000)
      score [pos].moves = moves; 
  }
  
 
  debug ("nach neupositionierung\n");

  // now draw that widget
  QGridLayout *lay = new QGridLayout (this, 1, 1, 10);

  n = new QLabel *[6];

  for (int i = 0; i < 6; i++)
  {
    // position + name ausgeben
    tmp.sprintf("%2d.   ", i + 1);
    tmp += score [i].name;
    n[i] = new QLabel(tmp, this);
    lay->addWidget(n[i], i, 0);
    
    // punkte ausgeben
    tmp.sprintf ("%d", score [i].moves);
    sc = new QLabel(tmp, this);
    sc->setAlignment( Qt::AlignRight );
    lay->addWidget(sc, i, 1);
  }

  lay->addRowSpacing(6, 20);

  // pushbutton erzeugen             
  ok = new QPushButton(i18n("OK"), this, "ok");
  lay->addMultiCellWidget ( ok, 6, 6, 0, 1);
  
  connect(ok, SIGNAL(clicked()), SLOT(accept()) );
  
  debug ("vor line edit\n ");

  // line edit erzeugen
  debug ("pos %d", pos);

  if (pos < 6 && moves > 0)
  {
    debug ("line edit erzeugen");
    le = new QLineEdit (this, "le");
    le->setMaxLength (18);
    lay->addWidget(le, pos, 0);
    connect (le, SIGNAL (returnPressed ()), 
             this, SLOT (eingabeFertig ()));

    // wenn position in highscore liste erreicht, dann ok button verstecken
    ok->hide ();

  } 
  
  debug ( "ende consturctor\n");
}

void Highscore::getChangedText (const QString& s)
{
  debug("%s eingabe fertig\n", s.ascii()); 
}

void Highscore::eingabeFertig ()
{
  debug ("\nfinished\n");

  // eingegebener text in s 

  QString s = le->text ();
  score [pos].name = s;
  saveHighscore();

  delete le;
  le = 0;

  // s als label ausgeben
  QString l; l.sprintf("%2d.   ", pos+1);
  l += s;
  n[pos]->setFont(QFont("Helvetica",12, QFont::Bold));
  n[pos]->setText(l);
  
  // pushbutton ok anzeigen
  ok->setFocus();
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
	score[i].moves = config->readNumEntry(key, (i+1)  * 100);
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
  debug ( "in destructor\n");
  delete [] n;
}

#include "highscore.moc" 
