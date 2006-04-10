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

#include <config.h>

#include "molek.moc"
#include <kiconloader.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <ksimpleconfig.h>
#include <klocale.h>
#include <kdebug.h>

#include <ctype.h>
//Added by qt3to4:
#include <QPaintEvent>

extern int level;

Molek::Molek( QWidget *parent, const char *name ) : QWidget( parent, name ),
                                                    data(locate("appdata", "pics/molek.png"))
{
    setBackgroundColor (QColor (0, 0, 0));
    setMinimumSize(240, 200);
}

Molek::~Molek ()
{
}

const atom& Molek::getAtom(int index) const
{
  static atom none = { 0, "" };

  if (index > atoms.count() || index == 0)
    return none;

  return atoms.at(index - 1);
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
	value = config.readEntry(key,QString());
	if (value.isEmpty())
	    break;

	current.obj = value.at(0).latin1();
	value = value.mid(2);
	if (value.isNull())
           value = "";

	strlcpy(current.conn, value.ascii(), sizeof(current.conn));
	kWarning( atoms.find(current) != atoms.end() )
	  << "OOOPS, duplicate atom definition in " << key << endl;
	atoms.append(current);
	atom_index++;
    }

    QString line;

    for (int j = 0; j < MOLEK_SIZE; j++) {

	key.sprintf("mole_%d", j);
	line = config.readEntry(key,QString());

	for (int i = 0; i < MOLEK_SIZE; i++)
	{
            if (i >= line.size()) molek[i][j] = 0;
	    else molek[i][j] = atom2int(line.at(i).latin1());
	}
    }

    mname = i18n(config.readEntry("Name", I18N_NOOP("Noname")).latin1());

    int& height = _size.rheight();
    int& width = _size.rwidth();

    height = 0;
    width = 0;

    for (int i = 0; i < MOLEK_SIZE; i++)
		for (int j = 0; j < MOLEK_SIZE; j++) {
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
    QString st = i18n("Level: %1", level);

    QPainter paint (this);
    paint.setPen (QColor (190, 190, 190));
    paint.drawText (7, height() - 36, mname);
    paint.drawText (7, height() - 18, st);
    // spielfeld gleich zeichnen
    for (int i = 0; i < MOLEK_SIZE; i++)
		for (int j = 0; j < MOLEK_SIZE; j++) {
			int x = 10 + i * 15;
			int y = 10 + j * 15;

			if (molek[i][j] == 0)
				continue;

			// paints atoms
			if (getAtom(molek [i] [j]).obj <= '9' && getAtom(molek [i] [j]).obj >= '1')
				bitBlt (this, x, y, &data, (getAtom(molek [i] [j]).obj - '1') * 15, 0, 15,
						15);

			// paints cristals
			if (getAtom(molek [i] [j]).obj == 'o')
				bitBlt (this, x, y, &data, 10 * 15, 0, 15, 15);

			// paints connections
			if (isdigit(getAtom(molek[i][j]).obj) || getAtom(molek[i][j]).obj == 'o')
				for (int c = 0; c < MAX_CONNS_PER_ATOM; c++) {
					char conn = getAtom(molek [i] [j]).conn[c];
					if (!conn)
						break;

					if (conn >= 'a' && conn <= 'a' + 8)
					{
						bitBlt (this, x, y, &data, (conn - 'a') * 15, 16, 15, 15);
					}
					else
					{
						bitBlt (this, x, y, &data, (conn - 'A') * 15, 34, 15, 15);
					}
				}


			// paints connections
			if (getAtom(molek[i][j]).obj >= 'A' && getAtom(molek[i][j]).obj <= 'F')
				bitBlt (this, x, y, &data, (getAtom(molek[i][j]).obj - 'A' + 11) * 15 , 0, 15, 15);

		}

    paint.end ();
}
