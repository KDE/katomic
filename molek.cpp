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
#include <ctype.h>

extern int hexValue(char ch);
extern int level;

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
      
      key.sprintf("mole_obje_%0d", i);
      const QString obj_line = config.readEntry(key);
   
      key.sprintf("mole_verb_%0d", i);
      const QString verb_line = config.readEntry(key);

      if (verb_line.isEmpty())
	  continue;
      if (obj_line.isEmpty())
	  continue;
      
      int verb_index = 0;

      for (int j = 0; j < 10; j++)
	  { 
	      
	      molek [i] [j].obj = obj_line.at(j);

	      molek [i][j].verb = 
		  hexValue(verb_line.at(verb_index++)) * 4096 + 
		  hexValue(verb_line.at(verb_index++)) * 256 + 
		  hexValue(verb_line.at(verb_index++)) * 16 + 
		  hexValue(verb_line.at(verb_index++));
	      
	  }
      
  }
  
  mname = config.readEntry("Name", i18n("Noname"));

  // höhe und breite des moleküls berechnen und ausgeben, checkdone 
  for (int i = 0, breite = 0, hohe = 0; i < 10; i++)
      for (int j = 0; j < 10; j++) {
	  if ((molek [i] [j].verb != 0) && (i > breite)) breite = i;
	  if ((molek [i] [j].verb != 0) && (j > hohe)) hohe = j;
      }
  
  repaint ();
}

void Molek::paintEvent( QPaintEvent * )
{
  QString st = i18n("Level: %1").arg(level);

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
    if (molek [i] [j].obj <= '9' && molek [i] [j].obj >= '1')
    {
      bitBlt (this, x, y, &data, (molek [i] [j].obj - '1') * 15, 0, 15, 
              15, CopyROP);
    }

    
    // zeichnet Kristalle
    if (molek [i] [j].obj == 'o')
    {
      bitBlt (this, x, y, &data, 10 * 15, 0, 15, 15, CopyROP);
    }
    

    // verbindungen zeichnen
    if (isdigit(molek[i][j].obj) || molek [i] [j].obj == 'o')
    {
      char anz;
      for (anz = 0; anz < 16; anz++)
        if ((molek [i] [j].verb & (1 << anz)) == (uint(1) << anz))
        {
          if (anz < 8)
            bitBlt (this, x, y, &data, anz * 15, 16, 15, 15, XorROP);
          else
            bitBlt (this, x, y, &data, (anz - 8) * 15, 34, 15, 15, XorROP);
        }
    }

    
    // zeichnet Verbindungsstäbe 
    if (molek [i] [j].obj >= 'A' && molek [i] [j].obj <= 'F')
      bitBlt (this, x, y, &data, (molek [i] [j].obj - 'A') * 15 , 0, 15, 15, 
              CopyROP);
    
  }
  
  paint.end ();
}
