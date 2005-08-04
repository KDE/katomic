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
#include <QAbstractEventDispatcher>
//Added by qt3to4:
#include <QPixmap>
#include <QMouseEvent>
#include <QTimerEvent>
#include <QPaintEvent>
#include "molek.h"
#include "feld.h"
#include "settings.h"

#if FIELD_SIZE < MOLEK_SIZE
#error Molecule size (MOLEK_SIZE) must be <= field size (FIELD_SIZE)
#endif

extern Options settings;

Feld::Feld( QWidget *parent, const char *name ) :
    QWidget( parent, name ),
    data(locate("appdata", "pics/abilder.png")),
    undoBegin (0), undoSize (0), redoSize (0)
{
    anim = false;
    dir = None;
    sprite = QPixmap (30, 30);

    cx = -1;
    cy = -1;

    point = new QPoint [1];

    moving = false;
    chosen = false;

    setMouseTracking(true);

    setFocusPolicy(Qt::StrongFocus);
    setBackgroundColor( QColor( 0, 0, 0) );

    setFixedSize(15 * 30, 15 * 30);
    copy = QPixmap(15 * 30, 15 * 30);
}

Feld::~Feld ()
{
  delete [] point;
}

void Feld::resetValidDirs()
{
  bool up = false;
  for (int j = 0; j < FIELD_SIZE; j++)
    for (int i = 0; i < FIELD_SIZE; i++)
      if (feld[i][j] >= 150 && feld[i][j] <= 153)
	{
	  feld[i][j] = 0;
	  up = true;
	}
  if (up) update();
}

void Feld::load (const KSimpleConfig& config)
{
  if(moving)
    QAbstractEventDispatcher::instance()->unregisterTimers(this);

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

  undoSize = redoSize = undoBegin = 0;
  emit enableUndo(false);
  emit enableRedo(false);

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
      update();
    }

    if (ypos < FIELD_SIZE-1 && feld[xpos][ypos+1] == 0) {
      feld [xpos][ypos+1] = 152;
      update();
    }

    if (xpos > 0 && feld[xpos-1][ypos] == 0) {
      feld [xpos-1][ypos] = 151;
      update();
    }

    if (xpos < FIELD_SIZE-1 && feld[xpos+1][ypos] == 0) {
      feld [xpos+1][ypos] = 153;
      update();
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
  repaint();

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

    // BEGIN: Insert undo informations
    uint undoChunk = (undoBegin + undoSize) % MAX_UNDO;
    undo[undoChunk].atom = feld[xpos][ypos];
    undo[undoChunk].oldxpos = xpos;
    undo[undoChunk].oldypos = ypos;
    undo[undoChunk].xpos = x;
    undo[undoChunk].ypos = y;
    undo[undoChunk].dir = dir;
    if (undoSize == MAX_UNDO)
      undoBegin = (undoBegin + 1) % MAX_UNDO;
    else
      ++undoSize;
    redoSize = undoSize;
    emit enableUndo(true);
    emit enableRedo(false);
    // END: Insert undo informations

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

    bitBlt (&sprite, 0, 0, &copy, cx, cy, 30, 30);
  }

}

void Feld::doUndo ()
{
  if (moving || !chosen || undoSize == 0)
    return;

  UndoInfo &undo_info = undo[(undoBegin + --undoSize) % MAX_UNDO];
  emit enableUndo(undoSize != 0);
  emit enableRedo(true);

  --moves;
  emit sendMoves(moves);

  moving = true;
  resetValidDirs ();

  cx = undo_info.xpos;
  cy = undo_info.ypos;
  xpos = undo_info.oldxpos;
  ypos = undo_info.oldypos;
  feld[cx][cy] = 0;
  feld[xpos][ypos] = undo_info.atom;
  cx *= 30; cy *= 30;
  framesbak = frames =
    30 * (abs (undo_info.xpos - undo_info.oldxpos) +
          abs (undo_info.ypos - undo_info.oldypos) );
  startTimer (10);
  dir = (Direction) -((int) undo_info.dir);
  bitBlt (&sprite, 0, 0, &copy, cx, cy, 30, 30);
}

void Feld::doRedo ()
{
  if (moving || !chosen || undoSize == redoSize)
    return;

  UndoInfo &undo_info = undo[(undoBegin + undoSize++) % MAX_UNDO];

  emit enableUndo(true);
  emit enableRedo(undoSize != redoSize);

  ++moves;
  emit sendMoves(moves);

  moving = true;
  resetValidDirs ();

  cx = undo_info.oldxpos;
  cy = undo_info.oldypos;
  xpos = undo_info.xpos;
  ypos = undo_info.ypos;
  feld[cx][cy] = 0;
  feld[xpos][ypos] = undo_info.atom;
  cx *= 30; cy *= 30;
  framesbak = frames =
    30 * (abs (undo_info.xpos - undo_info.oldxpos) +
          abs (undo_info.ypos - undo_info.oldypos) );
  startTimer (10);
  dir = undo_info.dir;
  bitBlt (&sprite, 0, 0, &copy, cx, cy, 30, 30);
}

void Feld::mouseMoveEvent (QMouseEvent *e)
{
  // warning: mouseMoveEvents can report positions upto 1 pixel outside
  //          of the field widget, so we must be sure handle this case

  if( e->pos().x() < 0 || e->pos().x() >= 450 ||
      e->pos().y() < 0 || e->pos().y() >= 450 )
  {
    setCursor(Qt::arrowCursor);
  }
  else
  {
    int x = e->pos ().x () / 30;
    int y = e->pos ().y () / 30;

    // verschiedene cursor je nach pos
    if (feld[x][y] != 254 && feld [x] [y] != 0)
      setCursor (Qt::crossCursor);
    else
      setCursor (Qt::arrowCursor);
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
	  QAbstractEventDispatcher::instance()->unregisterTimers(this);
	  done();
	  dir = None;
	}
  else
  {
    frames -= settings.anim_speed;
    if (frames < 0)
	  frames = 0;

    update();
  }
}

void Feld::paintMovingAtom(QPainter &paint)
{
	int a = settings.anim_speed;

	switch(dir)
	{
	case MoveUp:
		paint.drawPixmap(cx, cy - framesbak + frames, sprite);
		if(framesbak - frames > 0)
			paint.fillRect(cx, cy - framesbak + frames + 30, 30, a, Qt::black);
		break;
	case MoveDown:
		paint.drawPixmap(cx, cy + framesbak - frames, sprite);
		if(framesbak - frames > 0)
			paint.fillRect(cx, cy + framesbak - frames - a, 30, a, Qt::black);
		break;
	case MoveRight:
		paint.drawPixmap(cx + framesbak - frames, cy, sprite);
		if(framesbak - frames > 0)
			paint.fillRect(cx + framesbak - frames - a, cy, a, 30, Qt::black);
		break;
	case MoveLeft:
		paint.drawPixmap(cx - framesbak + frames, cy, sprite);
		if(framesbak - frames > 0)
			paint.fillRect(cx - framesbak + frames + 30, cy, a, 30, Qt::black);
		break;
    case None:
        break;
    }
}

void Feld::putNonAtom (int x, int y, Direction which, QPainter &p, bool brick)
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

  p.drawPixmap(x * 30, y * 30, data, xarr, yarr, 30, 30);
}

void Feld::paintEvent( QPaintEvent * )
{
    int i, j, x, y;

    QPainter paint ( &copy );

    if (moving)
    {
        paintMovingAtom(paint);
        bitBlt(this, 0, 0, &copy);
        return;
    }

    paint.fillRect(0, 0, 15 * 30, 15 * 30, Qt::black);
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
		      putNonAtom(i, j, Feld::None, paint, true); continue;
		    }

		    if (feld[i][j] == 150) {
		      putNonAtom(i, j, Feld::MoveUp, paint); continue;
		    }

		    if (feld[i][j] == 151) {
		      putNonAtom(i, j, Feld::MoveLeft, paint); continue;
		    }
		    if (feld[i][j] == 152) {
		      putNonAtom(i, j, Feld::MoveDown, paint); continue;
		    }

		    if (feld[i][j] == 153) {
		      putNonAtom(i, j, Feld::MoveRight, paint); continue;
		    }

		    // zeichnet Atome
		    if (getAtom(feld [i] [j]).obj <= '9' && getAtom(feld [i] [j]).obj > '0')
			{
			    paint.drawPixmap(x, y, data, (getAtom(feld [i] [j]).obj - '1') * 31, 0, 30,
				    30);
			}

		    // zeichnet Kristalle
		    if (getAtom(feld [i] [j]).obj == 'o')
			{
			    paint.drawPixmap(x, y, data, 31, 93, 30, 30);
			}



		    // verbindungen zeichnen
		    if (getAtom(feld [i] [j]).obj <= '9' ||
			getAtom(feld [i] [j]).obj == 'o')
			for (int c = 0; c < MAX_CONNS_PER_ATOM; c++) {
			    char conn = getAtom(feld [i] [j]).conn[c];
			    if (!conn)
				break;

			    if (conn >= 'a' && conn <= 'a' + 8)
				paint.drawPixmap(x, y,
					data, (conn - 'a') * 31, 31, 30, 30);
			    else
				paint.drawPixmap(x, y,
					data, (conn - 'A') * 31, 62, 30, 30);

			}

		    // zeichnet Verbindungsstäbe
		    if (getAtom(feld [i] [j]).obj >= 'A' &&
			getAtom(feld [i] [j]).obj <= 'F')
			paint.drawPixmap(x, y,
				data,
				(getAtom(feld [i] [j]).obj - 'A' + 2) * 31 ,
				93, 30, 30);
		}
    }
    bitBlt(this, 0, 0, &copy);
}


#include "feld.moc"
