/****************************************************************
**
** Implementation Molek class, derieved from Qt tutorial 8
**
****************************************************************/

// bemerkungen : wenn paintEvent aufgerufen wird, wird das komplette
//               widget gelöscht und nur die sachen gezeichnet, die in 
//               paintEvent stehen ! sollen dinge z.b nur bei maustasten-
//               druck gezeichnet werden, so muß dies in mousePressEvent 
//               stehen !  
//               paintEvent wird aufgerufen, falls fenster überdeckt wird,
//               oder auch einfach bewegt wird 




#include "molek.moc"
#include <kiconloader.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <ksimpleconfig.h>
#include <klocale.h>

extern int hexValue(char ch);

Molek::Molek( QWidget *parent, const char *name ) : QWidget( parent, name )
{
  data = ICON("molek.png");
}

Molek::~Molek ()
{ 
}

void Molek::loadFeldFromDat (const KSimpleConfig& config)
{
  QString key;
  for (int i = 0; i < 10; i++) {
    
    key = QString("mole_obje_%1").arg(i);
    const QString obj_line = config.readEntry(key);
    key = QString("mole_verb_%1").arg(i);
    const QString verb_line = config.readEntry(key);
    
    if (verb_line.isEmpty())
      continue;
    if (obj_line.isEmpty())
      continue;

    int verb_index = 0;
    int obj_index = 0;

    QString line1, line2;

    for (int j = 0; j < 10; j++)
      { 
	molek [i] [j].obj = 
	  hexValue(obj_line.at(obj_index++)) * 16 + 
	  hexValue(obj_line.at(obj_index++));
	
	molek [i][j].verb = 
	  hexValue(verb_line.at(verb_index++)) * 4096 + 
	  hexValue(verb_line.at(verb_index++)) * 256 + 
	  hexValue(verb_line.at(verb_index++)) * 16 + 
	  hexValue(verb_line.at(verb_index++));

      }

    }

  mname = config.readEntry("Name", i18n("Noname"));

  repaint ();
}

void Molek::paintEvent( QPaintEvent * )
{
  int level = 1;
  QString st = i18n("Level : %1").arg(level);

  QPainter paint (this);
  paint.setPen (QColor (190, 190, 190));
  paint.drawText (7, 152, mname);
  paint.drawText (7, 170, st);
  // spielfeld gleich zeichnen 
  for (int i = 0; i < 10; i++)
  for (int j = 0; j < 10; j++)
  {
    int x = 10 + i * 15;
    int y = 10 + j * 15;

    // zeichnet Atome
    if (molek [i] [j].obj < 11 && molek [i] [j].obj > 0)
    {
      bitBlt (this, x, y, &data, (molek [i] [j].obj - 1) * 15, 0, 15, 
              15, CopyROP);
    }

    
    // zeichnet Kristalle
    if (molek [i] [j].obj == 20)
    {
      bitBlt (this, x, y, &data, 10 * 15, 0, 15, 15, CopyROP);
    }
    

    // verbindungen zeichnen
    if (molek [i] [j].obj < 11 || molek [i] [j].obj == 20)
    {
      char anz;
      for (anz = 0; anz < 16; anz++)
        if ((molek [i] [j].verb & (1 << anz)) == (1 << anz))
        {
          if (anz < 8)
            bitBlt (this, x, y, &data, anz * 15, 16, 15, 15, XorROP);
          else
            bitBlt (this, x, y, &data, (anz - 8) * 15, 34, 15, 15, XorROP);
        }
    }

    
    // zeichnet Verbindungsstäbe 
    if (molek [i] [j].obj > 29 && molek [i] [j].obj < 34)
      bitBlt (this, x, y, &data, (molek [i] [j].obj - 19) * 15 , 0, 15, 15, 
              CopyROP);
    
  }
  
  paint.end ();
}
