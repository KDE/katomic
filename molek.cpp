/****************************************************************
**
** Implementation Molek class, derieved from Qt tutorial 8
**
****************************************************************/

// bemerkungen : wenn paintEvent aufgerufen wird, wird das komplette
//               widget gel�scht und nur die sachen gezeichnet, die in 
//               paintEvent stehen ! sollen dinge z.b nur bei maustasten-
//               druck gezeichnet werden, so mu� dies in mousePressEvent 
//               stehen !  
//               paintEvent wird aufgerufen, falls fenster �berdeckt wird,
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

void Molek::load (const KSimpleConfig& config)
{
    atoms.clear();
    QString key;

    atom none;
    none.obj = 0;
    none.conn = 0;
    atoms.append(none);

    atom current;
    
    int atom_index = 1;
    QString value;
    while (true) {
	key.sprintf("atom_%c", int2atom(atom_index));
	value = config.readEntry(key);
	if (value.isEmpty())
	    break;
	
	current.obj = value.at(0).latin1();
	value = value.mid(2);
	current.conn = value.toInt(0, 16);
	atoms.append(current);
	atom_index++;
    }

    QString line;

    for (int j = 0; j < 10; j++) {
      
	key.sprintf("mole_%d", j);
	line = config.readEntry(key);

	for (int i = 0; i < 10; i++)
	    molek[i][j] = atom2int(line.at(i).latin1());
    }

    mname = config.readEntry("Name", i18n("Noname"));
    
#if 0
    int hohe = 0;
    int breite = 0;

    // h�he und breite des molek�ls berechnen und ausgeben, checkdone 
    for (int i = 0; i < 10; i++)
	for (int j = 0; j < 10; j++) {
	    if (getAtom(molek [i] [j]).conn == 0)
		continue;
	    // debug("%x", getAtom(molek [i] [j])).conn);
	    if (i > breite) breite = i;
	    if (j > hohe) hohe = j;
	}
    hohe++;
    breite++;
    
    debug("%d %d", hohe, breite);
#endif

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
    if (getAtom(molek [i] [j]).obj <= '9' && getAtom(molek [i] [j]).obj >= '1')
    {
      bitBlt (this, x, y, &data, (getAtom(molek [i] [j]).obj - '1') * 15, 0, 15, 
              15, CopyROP);
    }

    
    // zeichnet Kristalle
    if (getAtom(molek [i] [j]).obj == 'o')
    {
      bitBlt (this, x, y, &data, 10 * 15, 0, 15, 15, CopyROP);
    }
    

    // verbindungen zeichnen
    if (isdigit(getAtom(molek[i][j]).obj) || getAtom(molek[i][j]).obj == 'o')
    {
      char anz;
      for (anz = 0; anz < 16; anz++)
        if ((getAtom(molek[i][j]).conn & (1 << anz)) == (uint(1) << anz))
        {
          if (anz < 8)
            bitBlt (this, x, y, &data, anz * 15, 16, 15, 15, XorROP);
          else
            bitBlt (this, x, y, &data, (anz - 8) * 15, 34, 15, 15, XorROP);
        }
    }

    
    // zeichnet Verbindungsst�be 
    if (getAtom(molek[i][j]).obj >= 'A' && getAtom(molek[i][j]).obj <= 'F')
      bitBlt (this, x, y, &data, (getAtom(molek[i][j]).obj - 'A') * 15 , 0, 15, 15, 
              CopyROP);
    
  }
  
  paint.end ();
}
