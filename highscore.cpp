/* highscore.cpp
 *
 * Andreas Wüst
 *
 */

#include "highscore.h"
#include <stdlib.h>
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

  if (moves > 0)
  {
    int i = 5;
    while (moves <= score [i].moves && i >= 0)
    {
      // this is the position where the new score goes
      pos = i;
      i--;
    }


    // nachfolgende positionen verschieben
    i = 5;
    while (i > pos && i > 0)
    {
      score [i].name = score [i - 1].name;
      score [i].moves = score [i - 1].moves;
      i--;
    }
    if (pos != 1000)
      score [pos].moves = moves; 
  }
  
 

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
    tmp.setNum(score [i].moves);
    sc = new QLabel(tmp, this);
    sc->setAlignment( Qt::AlignRight );
    lay->addWidget(sc, i, 1);
  }

  lay->addRowSpacing(6, 20);

  // pushbutton erzeugen             
  ok = new QPushButton(i18n("&OK"), this, "ok");
  ok->setDefault(true);
  lay->addMultiCellWidget ( ok, 6, 6, 0, 1);
  
  connect(ok, SIGNAL(clicked()), SLOT(accept()) );
  
  if (pos < 6 && moves > 0)
  {
    const int maxLen = 18; // not yet sure why this is 18, but...
    QString username = getenv("LOGNAME");
    le = new QLineEdit (this, "le");
    le->setText(username);
    le->setFocus();
    le->setMaxLength(maxLen);
    le->setCursorPosition(maxLen);
    le->setSelection(0, maxLen);
    lay->addWidget(le, pos, 0);
    connect (le, SIGNAL (returnPressed ()), 
             this, SLOT (eingabeFertig ()));

    // wenn position in highscore liste erreicht, dann ok button verstecken
    ok->hide ();

  } 
  
}

void Highscore::getChangedText (const QString& s)
{
}

void Highscore::eingabeFertig ()
{

  // eingegebener text in s 

  QString s = le->text ();
  score [pos].name = s;
  saveHighscore();

  // at least Qt 2.1beta doesn't like this
//  delete le;
//  le = 0;

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
  delete [] n;
}

#include "highscore.moc" 
