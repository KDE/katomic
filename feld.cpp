/****************************************************************
**
** Implementation Feld class, derieved from Qt tutorial 8
**
****************************************************************/

// bemerkungen : wenn paintEvent aufgerufen wird, wird das komplette
//               widget gelöscht und nur die sachen gezeichnet, die in
//               paintEvent stehen ! sollen dinge z.b nur bei maustasten-
//               druck gezeichnet werden, so muß dies in mousePressEvent
//               stehen !
//               paintEvent wird aufgerufen, falls fenster überdeckt wird,
//               oder auch einfach bewegt wird

#include <kiconloader.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <ksimpleconfig.h>
#include "molek.h"
#include "feld.h"
#include "settings.h"

#if FIELD_SIZE < MOLEK_SIZE
#error Molecule size (MOLEK_SIZE) must be <= field size (FIELD_SIZE)
#endif

extern Options settings;

Feld::Feld( QWidget *parent, const char *name ) :
    QWidget( parent, name )
{
    anim = false;
    dir = None;
    sprite = QPixmap (30, 30);

    cx = -1;
    cy = -1;

    point = new QPoint [1];
    data = BarIcon("abilder");

    moving = false;
    chosen = false;

    setMouseTracking(true);

    setFocusPolicy(QWidget::StrongFocus);
    setBackgroundColor( QColor( 0, 0, 0) );

    setFixedSize(15 * 30, 15 * 30);
}

Feld::~Feld ()
{
  delete [] point;
}

void Feld::resetValidDirs()
{
  for (int j = 0; j < FIELD_SIZE; j++)
    for (int i = 0; i < FIELD_SIZE; i++)
      if (feld[i][j] >= 150 && feld[i][j] <= 153)
	{
	  feld[i][j] = 0;
	  putNonAtom(i,j, Feld::None);
	}
}

void Feld::load (const KSimpleConfig& config)
{
  if(moving)
    killTimers();

  mol->load(config);

  QString key;

  for (int j = 0; j < FIELD_SIZE; j++) {

    key.sprintf("feld_%02d", j);
    QString line = config.readEntry(key);

    for (int i = 0; i < FIELD_SIZE; i++)
	feld[i][j] = atom2int(line[i].latin1());

  }

  moves = 0;
  chosen = false;
  moving = false;

  xpos = ypos = 0;
  nextAtom();
}

void Feld::mousePressEvent (QMouseEvent *e)
{
  if (moving)
    return;

  int x = e->pos ().x () / 30;
  int y = e->pos ().y () / 30;

  if ( feld [x] [y] == 150)
    startAnimation (Feld::MoveUp);
  else if ( feld [x] [y] == 151)
    startAnimation (Feld::MoveLeft);
  else if ( feld [x] [y] == 152)
    startAnimation (Feld::MoveDown);
  else if ( feld [x] [y] == 153)
    startAnimation (Feld::MoveRight);
  else if (feld [x] [y] != 254 && feld [x] [y] != 0) {
    chosen = true;
    xpos = x;
    ypos = y;
    dir = None;
    resetValidDirs();
  } else {
    resetValidDirs();
    chosen = false;
  }
  emitStatus();
}

const atom& Feld::getAtom(uint index) const
{
  return mol->getAtom(index);
}


void Feld::nextAtom()
{
  int x = xpos, y;

  // make sure we don't check the current atom :-)
  if (ypos++ >= 15) ypos = 0;

  while(1)
    {
      for (y = ypos; y < FIELD_SIZE; y++)
	{
	  if ( feld [x] [y] != 0 &&
	       feld [x] [y] != 254 &&
	       feld [x] [y] != 150 &&
	       feld [x] [y] != 151 &&
	       feld [x] [y] != 152 &&
	       feld [x] [y] != 153 )
	    {
	      xpos = x; ypos = y;
	      chosen = true;
	      resetValidDirs();
	      emitStatus();
	      return;
	    }
	}
      ypos = 0;
      x++;
      if (x >= FIELD_SIZE) x = 0;
    }

}


void Feld::previousAtom()
{
  int x = xpos, y;

  // make sure we don't check the current atom :-)
  if (ypos-- <= 0) ypos = FIELD_SIZE-1;

  while(1)
    {
      for (y = ypos; y >= 0; y--)
	{
	  if ( feld [x] [y] != 0 &&
	       feld [x] [y] != 254 &&
	       feld [x] [y] != 150 &&
	       feld [x] [y] != 151 &&
	       feld [x] [y] != 152 &&
	       feld [x] [y] != 153 )
	    {
	      xpos = x; ypos = y;
	      chosen = true;
	      resetValidDirs();
	      emitStatus();
	      return;
	    }
	}
      ypos = FIELD_SIZE-1;
      x--;
      if (x <= 0) x = FIELD_SIZE-1;
    }
}


void Feld::emitStatus()
{
  if (!chosen || moving) {}
  else {

    if (ypos > 0 && feld[xpos][ypos-1] == 0) {
      feld [xpos][ypos-1] = 150;
      putNonAtom(xpos, ypos-1, Feld::MoveUp);
    }

    if (ypos < FIELD_SIZE-1 && feld[xpos][ypos+1] == 0) {
      feld [xpos][ypos+1] = 152;
      putNonAtom(xpos, ypos+1, Feld::MoveDown);
    }

    if (xpos > 0 && feld[xpos-1][ypos] == 0) {
      feld [xpos-1][ypos] = 151;
      putNonAtom(xpos-1, ypos, Feld::MoveLeft);
    }

    if (xpos < FIELD_SIZE-1 && feld[xpos+1][ypos] == 0) {
      feld [xpos+1][ypos] = 153;
      putNonAtom(xpos+1, ypos, Feld::MoveRight);
    }

  }
}

void Feld::done ()
{
  if (moving)
    return;

  emitStatus();

  if (checkDone())
    emit gameOver(moves);

}

void Feld::startAnimation (Direction d)
{
  // if animation is already started, return
  if (moving || !chosen)
    return;

  switch (d) {
  case MoveUp:
	if (ypos == 0 || feld [xpos] [ypos-1] != 150)
	  return;
	break;
  case MoveDown:
	if (ypos == FIELD_SIZE-1 || feld [xpos] [ypos+1] != 152)
	  return;
	break;
  case MoveLeft:
	if (xpos == 0 || feld [xpos-1] [ypos] != 151)
	  return;
	break;
  case MoveRight:
	if (xpos == FIELD_SIZE-1 || feld [xpos+1] [ypos] != 153)
	  return;
	break;
  default:
	break;
  }

  // reset validDirs now so that arrows don't get drawn
  resetValidDirs();

  int x = 0, y = 0;

  moves++;
  emit sendMoves(moves);
  dir = d;

  switch (dir) {
  case MoveUp :
    for (x = xpos, y = ypos-1, anz = 0; y >= 0 && feld [x] [y] == 0; anz++, y--);
    if (anz != 0)
      {
	feld [x] [++y] = feld [xpos] [ypos];
      }
    break;
  case MoveDown :
    for (x = xpos, y = ypos+1, anz = 0; y <= FIELD_SIZE-1 && feld [x] [y] == 0; anz++, y++);
    if (anz != 0)
      {
	feld [x] [--y] = feld [xpos] [ypos];
      }
    break;
  case MoveRight :
    for (x = xpos+1, y = ypos, anz = 0; x <= FIELD_SIZE-1 && feld [x] [y] == 0; anz++, x++);
    if (anz != 0)
      {
	feld [--x] [y] = feld [xpos] [ypos];
      }
    break;
  case MoveLeft :
    for (x = xpos-1, y = ypos, anz = 0; x >= 0 && feld [x] [y] == 0; anz++, x--);
    if (anz != 0)
      {
	feld [++x] [y] = feld [xpos] [ypos];
      }
    break;
  default:
    return;
  }

  if (anz != 0) {
    moving = true;
    feld [xpos] [ypos] = 0;

    // absolutkoordinaten des zu verschiebenden bildes
    cx = xpos * 30;
    cy = ypos * 30;
    xpos = x;
    ypos = y;
    // 30 animationsstufen
    framesbak = frames = anz * 30;

    // 10 mal pro sek
    startTimer (10);

    bitBlt (&sprite, 0, 0, this, cx, cy, 30, 30, CopyROP);
  }

}



void Feld::mouseMoveEvent (QMouseEvent *e)
{
  // warning: mouseMoveEvents can report positions upto 1 pixel outside
  //          of the field widget, so we must be sure handle this case

  if( e->pos().x() < 0 || e->pos().x() >= 450 ||
      e->pos().y() < 0 || e->pos().y() >= 450 )
  {
    setCursor(arrowCursor);
  }
  else
  {
    int x = e->pos ().x () / 30;
    int y = e->pos ().y () / 30;

    // verschiedene cursor je nach pos
    if (feld[x][y] != 254 && feld [x] [y] != 0)
      setCursor (crossCursor);
    else
      setCursor (arrowCursor);
  }
}


bool Feld::checkDone ()
{
	int molecWidth = mol->molecSize().width();
	int molecHeight = mol->molecSize().height();
	int i = 0;
	int j = 0;

	// find first atom in molecule
	uint firstAtom = 0;
	for(j = 0; j < molecHeight && !firstAtom; ++j)
		firstAtom = mol->getAtom(0, j);

	// wot no atom?
	if(!firstAtom)
		return true; // true skips to next level

	// position of first atom (in molecule coordinates)
	int mx = 0;
	int my = j - 1;

	QRect extent(0, 0, FIELD_SIZE - molecWidth + 1, FIELD_SIZE - molecHeight + 1);
	extent.moveBy(0, my);

	// find first atom in playing field
	for(i = extent.left(); i <= extent.right(); ++i)
	{
		for(j = extent.top(); j <= extent.bottom(); ++j)
		{
			if(feld[i][j] == firstAtom)
			{
				// attempt to match playing field to molecule
				int ox = i - mx; // molecule origin (in field coordinates)
				int oy = j - my; // molecule origin (in field coordinates)
				++my; // no need to test first atom again
				while(mx < molecWidth)
				{
					while(my < molecHeight)
					{
						uint nextAtom = mol->getAtom(mx, my);
						if(nextAtom != 0 && feld[ox + mx][oy + my] != nextAtom)
							return false;
						++my;
					}
					my = 0;
					++mx;
				}
				return true;
			}
		}
	}
	// if we got here, then the first atom is too low or too far right
	// for the molecule to be assembled correctly
	return false;
}


void Feld::timerEvent (QTimerEvent *)
{
  // animation beenden
  if (frames <= 0)
	{
	  moving = false;
	  killTimers ();
	  done();
	  dir = None;
	}
  else
  {
    frames -= settings.anim_speed;
    if (frames < 0)
	  frames = 0;

    paintMovingAtom();
  }
}

void Feld::paintMovingAtom()
{
	int a = settings.anim_speed;

	QPainter paint(this);

	switch(dir)
	{
	case MoveUp:
		bitBlt(this, cx, cy - framesbak + frames, &sprite, CopyROP);
		if(framesbak - frames > 0)
			paint.eraseRect(cx, cy - framesbak + frames + 30, 30, a);
		break;
	case MoveDown:
		bitBlt(this, cx, cy + framesbak - frames, &sprite, CopyROP);
		if(framesbak - frames > 0)
			paint.eraseRect(cx, cy + framesbak - frames - a, 30, a);
		break;
	case MoveRight:
		bitBlt(this, cx + framesbak - frames, cy, &sprite, CopyROP);
		if(framesbak - frames > 0)
			paint.eraseRect(cx + framesbak - frames - a, cy, a, 30);
		break;
	case MoveLeft:
		bitBlt(this, cx - framesbak + frames, cy, &sprite, CopyROP);
		if(framesbak - frames > 0)
			paint.eraseRect(cx - framesbak + frames + 30, cy, a, 30);
		break;
    case None:
        break;
    }
}

void Feld::putNonAtom (int x, int y, Direction which, bool brick)
{
  int xarr=0, yarr=0;
  switch (which)
    {
    case None      : xarr = 279, yarr = 31 * (brick?1:2); break;
    case MoveUp    : xarr = 248; yarr = 62; break;
    case MoveLeft  : xarr = 217; yarr = 93; break;
    case MoveDown  : xarr = 248; yarr = 93; break;
    case MoveRight : xarr = 279; yarr = 93; break;
    }

  bitBlt(this, x * 30, y * 30, &data, xarr, yarr, 30, 30, CopyROP);
}

void Feld::paintEvent( QPaintEvent * )
{
    int i, j, x, y;

    QPainter paint ( this );

    paint.setPen (black);

	// spielfeld gleich zeichnen

	for (i = 0; i < FIELD_SIZE; i++)
	{
	    for (j = 0; j < FIELD_SIZE; j++)
		{
		    if(moving && i == xpos && j == ypos)
		      continue;

		    x = i * 30;
		    y = j * 30;

		    // zeichnet Randstücke
		    if (feld [i] [j] == 254) {
		      putNonAtom(i, j, Feld::None, true); continue;
		    }

		    if (feld[i][j] == 150) {
		      putNonAtom(i, j, Feld::MoveUp); continue;
		    }

		    if (feld[i][j] == 151) {
		      putNonAtom(i, j, Feld::MoveLeft); continue;
		    }
		    if (feld[i][j] == 152) {
		      putNonAtom(i, j, Feld::MoveDown); continue;
		    }

		    if (feld[i][j] == 153) {
		      putNonAtom(i, j, Feld::MoveRight); continue;
		    }

		    // zeichnet Atome
		    if (getAtom(feld [i] [j]).obj <= '9' && getAtom(feld [i] [j]).obj > '0')
			{
			    bitBlt (this, x, y, &data, (getAtom(feld [i] [j]).obj - '1') * 31, 0, 30,
				    30, CopyROP);
			}

		    // zeichnet Kristalle
		    if (getAtom(feld [i] [j]).obj == 'o')
			{
			    bitBlt (this, x, y, &data, 31, 93, 30, 30, CopyROP);
			}



		    // verbindungen zeichnen
		    if (getAtom(feld [i] [j]).obj <= '9' ||
			getAtom(feld [i] [j]).obj == 'o')
			for (int c = 0; c < MAX_CONNS_PER_ATOM; c++) {
			    char conn = getAtom(feld [i] [j]).conn[c];
			    if (!conn)
				break;

			    if (conn >= 'a' && conn <= 'a' + 8)
				bitBlt (this, x, y,
					&data, (conn - 'a') * 31, 31, 30, 30,
					XorROP);
			    else
				bitBlt (this, x, y,
					&data, (conn - 'A') * 31, 62, 30, 30,
					XorROP);

			}

		    // zeichnet Verbindungsstäbe
		    if (getAtom(feld [i] [j]).obj >= 'A' &&
			getAtom(feld [i] [j]).obj <= 'F')
			bitBlt (this, x, y,
				&data,
				(getAtom(feld [i] [j]).obj - 'A' + 2) * 31 ,
				93, 30, 30,
				CopyROP);
		}
    }
}


#include "feld.moc"
