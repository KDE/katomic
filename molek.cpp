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

extern int level;

Molek::Molek( QWidget *parent, const char *name ) : QWidget( parent, name )
{
    data = ICON("molek.png");
}

Molek::~Molek ()
{ 
}

const atom& Molek::getAtom(uint index) const
{ 
  static atom none = { 0, "" };

  if (index >= atoms.count() || index == 0)
    return none;

  return *atoms.at(index - 1); 
}

void Molek::load (const KSimpleConfig& config)
{
    atoms.clear();
    QString key;

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

	strcpy(current.conn, value.ascii());
	if (atoms.find(current) != atoms.end()) {
	    warning("OOOPS");
	}
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

    int& height = _size.rheight();
    int& width = _size.rwidth();

    height = 0;
    width = 0;

    // höhe und breite des moleküls berechnen und ausgeben, checkdone 
    for (int i = 0; i < 10; i++)
	for (int j = 0; j < 10; j++) {
	    if (molek [i][j] == 0)
		continue;
	    if (i > width) width = i;
	    if (j > height) height = j;
	}
    height++;
    width++;
    
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
	for (int j = 0; j < 10; j++) {
	    int x = 10 + i * 15;
	    int y = 10 + j * 15;
	    
	    if (molek[i][j] == 0)
		continue;
	    
	    // zeichnet Atome
	    if (getAtom(molek [i] [j]).obj <= '9' && getAtom(molek [i] [j]).obj >= '1')
		bitBlt (this, x, y, &data, (getAtom(molek [i] [j]).obj - '1') * 15, 0, 15, 
			15, CopyROP);
	    
	    // zeichnet Kristalle
	    if (getAtom(molek [i] [j]).obj == 'o')
		bitBlt (this, x, y, &data, 10 * 15, 0, 15, 15, CopyROP);
	    
	    // verbindungen zeichnen
	    if (isdigit(getAtom(molek[i][j]).obj) || getAtom(molek[i][j]).obj == 'o')
		for (int c = 0; c < MAX_CONNS_PER_ATOM; c++) {
		    char conn = getAtom(molek [i] [j]).conn[c];
		    if (!conn)
			break;
		    
		    if (conn >= 'a' && conn <= 'a' + 8)
			bitBlt (this, x, y, &data, (conn - 'a') * 15, 16, 15, 15, XorROP);
		    else
			bitBlt (this, x, y, &data, (conn - 'A') * 15, 34, 15, 15, XorROP);
		    
		}
	    
	    
	    // zeichnet Verbindungsstäbe 
	    if (getAtom(molek[i][j]).obj >= 'A' && getAtom(molek[i][j]).obj <= 'F')
		bitBlt (this, x, y, &data, (getAtom(molek[i][j]).obj - 'A' + 11) * 15 , 0, 15, 15, 
			CopyROP);
	    
	}
    
    paint.end ();
}
