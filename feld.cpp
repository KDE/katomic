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

#include "highscore.h"
#include <kiconloader.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <ksimpleconfig.h>
#include "molek.h"
#include "feld.h"
#include "settings.h"

extern Options settings;

Feld::Feld( Molek *_mol, QWidget *parent, const char *name ) : 
  QWidget( parent, name )
{
  mol = _mol;
  anim = false;
  dir = None;
  sprite = QPixmap (30, 30);

  cx = -1;
  cy = -1;

  point = new QPoint [1];
  data = ICON("abilder.png");
 
  moving = false;
  chosen = false;

  resetValidDirs();

  setMouseTracking(true);

  setFocusPolicy(QWidget::StrongFocus);
}

Feld::~Feld ()
{ 
  delete [] point;
}

void Feld::resetValidDirs()
{ 
  for (int j = 0; j < 15; j++)
    for (int i = 0; i < 15; i++)
      if (feld[i][j] >= 150 && feld[i][j] <= 153)
	feld[i][j] = 0;
  repaint();
}

void Feld::load (const KSimpleConfig& config)
{
  mol->load(config);
  
  QString key;
  
  for (int j = 0; j < 15; j++) {
      
    key.sprintf("feld_%02d", j);
    const QString line = config.readEntry(key);

    for (int i = 0; i < 15; i++)
	feld[i][j] = atom2int(line.at(i));
    
  }

  moves = 0;
  chosen = false;
  moving = false;

  emitStatus();

  repaint ();
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

void Feld::keyPressEvent (QKeyEvent *e) 
{
  switch (e->key())
    {
    case Qt::Key_Up:
      //CT later, when we make this configurable    case Feld::UpKey:
      if (feld [xpos] [ypos-1] == 150)
	startAnimation(Feld::MoveUp); 
      break;
    case Qt::Key_Down:
      // case Feld::DownKey:
      if (feld [xpos] [ypos+1] == 152)
	startAnimation(Feld::MoveDown); 
      break;
    case Qt::Key_Left:
      // case Feld::LeftKey:
      if (feld [xpos-1] [ypos] == 151)
	startAnimation(Feld::MoveLeft); 
      break;
    case Qt::Key_Right:
      // case Feld::DownKey:
      if (feld [xpos+1] [ypos] == 153)
	startAnimation(Feld::MoveRight); 
      break;
    case Qt::Key_Tab:
      //CT this will do something in the future :-)
      nextAtom(); break;
    default:
      e->ignore();
    }
}


const atom& Feld::getAtom(uint index) const 
{ 
  return mol->getAtom(index);
}

void Feld::nextAtom() {}

void Feld::emitStatus()
{
  if (!chosen || moving) {}
  else {
    bool newones = false;

    if (feld[xpos][ypos-1] == 0) {
      feld [xpos][ypos-1] = 150;
      newones = true;
    }
    
    if (feld[xpos][ypos+1] == 0) {
      feld [xpos][ypos+1] = 152;
      newones = true;
    }
    
    if (feld[xpos-1][ypos] == 0) {
      feld [xpos-1][ypos] = 151;
      newones = true;
    }
    
    if (feld[xpos+1][ypos] == 0) {
      feld [xpos+1][ypos] = 153;
      newones = true;
    }
    
    if (newones)
      repaint();

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
  // wenn bereits animation stattfindet, nix machen
  if (moving || !chosen)
    return;

  // reset validDirs now so that arrows don't get drawn
  resetValidDirs();

  int x = 0, y = 0;

  moves++;
  emit sendMoves(moves);
  dir = d;
  
  switch (dir) {
  case MoveUp : 
    for (x = xpos, y = ypos, anz = 0; feld [x] [--y] == 0; anz++);
    if (anz != 0)
      {
	feld [x] [++y] = feld [xpos] [ypos];
      }
    break;
  case MoveDown : 
    for (x = xpos, y = ypos, anz = 0; feld [x] [++y] == 0; anz++);
    if (anz != 0)
      {
	feld [x] [--y] = feld [xpos] [ypos];
      }
    break;
  case MoveRight : 
    for (x = xpos, y = ypos, anz = 0; feld [++x] [y] == 0; anz++);
    if (anz != 0)
      {
	feld [--x] [y] = feld [xpos] [ypos];
      }
    break;
  case MoveLeft : 
    for (x = xpos, y = ypos, anz = 0; feld [--x] [y] == 0; anz++);
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
  int x = e->pos ().x () / 30;
  int y = e->pos ().y () / 30;

  // verschiedene cursor je nach pos
  if (feld[x][y] != 254 && feld [x] [y] != 0)
    setCursor (crossCursor);
  else
    setCursor (arrowCursor);
  
}


bool Feld::checkDone ()
{
  for (int i = 0; i < 15 - mol->molecSize().width(); i++)
  for (int j = 0; j < 15 - mol->molecSize().height(); j++)
  {
    bool done = true;
    if (feld [i] [j] == mol->getAtom(0,0))        // gleich links oben
    {
      for (int xx = 0; xx < mol->molecSize().width(); xx++)
      for (int yy = 0; yy < mol->molecSize().height(); yy++)
      {
        // ersten 3 bytes des strukturelements vergleichen (obj, verb)
        if (feld [i + xx ] [j + yy] != mol->getAtom(xx, yy) && feld[i + xx ] [j + yy] != 254)
          done = false;
      }
      if (done)
        return true;
    }
  }

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
    repaint (false);
  }
}


void Feld::paintEvent( QPaintEvent * )
{
    int i, j, x, y;
    
    QPainter paint ( this );
    
    paint.setPen (black);
    
    if (moving) {
	switch (dir) {
	case MoveUp : bitBlt (this, cx, cy - framesbak + frames, &sprite, CopyROP);
	    if ( (framesbak - frames > 1)  )
		{
		  paint.eraseRect (cx, cy - framesbak + frames + 30, 30, 2);
		}
	    break;
	case MoveDown : bitBlt (this, cx, cy + (framesbak - frames), &sprite, CopyROP);
	    if ( (framesbak - frames > 1) )
		{
		    paint.eraseRect (cx, cy + (framesbak - frames) - 2, 30, 2);
		}
	    break;
	case MoveRight : bitBlt (this, cx + (framesbak - frames), cy, &sprite, CopyROP);
	    if ( (framesbak - frames > 1) )
		{	 
		    paint.eraseRect (cx + (framesbak - frames) - 2, cy, 2, 30);
		}
	    break;
	case MoveLeft : bitBlt (this, cx - framesbak + frames, cy, &sprite, CopyROP);
	    if ((framesbak - frames > 1))
		{
		    paint.eraseRect (cx - framesbak + frames + 30, cy, 2, 30);
		}
	    break;
	default:
	  return;
	    
	}
	
    } else {
	
	// spielfeld gleich zeichnen 
	
	for (i = 0; i < 15; i++)
	    for (j = 0; j < 15; j++)
		{
		    x = i * 30;
		    y = j * 30;
		    
		    // zeichnet Randstücke
		    if (feld [i] [j] == 254) {
			bitBlt (this, x, y, &data, 279, 31, 30, 30, CopyROP);
			continue;
		    }

		    if (feld[i][j] == 150) {
		      bitBlt(this, x, y, &data, 248, 62, 30, 30, CopyROP);
		      continue;
		    }

		    if (feld[i][j] == 151) {
		      bitBlt(this, x, y, &data, 217, 93, 30, 30, CopyROP);
		      continue;
		    }
		    if (feld[i][j] == 152) {
		      bitBlt(this, x, y, &data, 248, 93, 30, 30, CopyROP);
		      continue;
		    }
		    
		    if (feld[i][j] == 153) {
		      bitBlt(this, x, y, &data, 279, 93, 30, 30, CopyROP);
		      continue;
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
		    if (getAtom(feld [i] [j]).obj <= '9' || getAtom(feld [i] [j]).obj == 'o')
			for (int c = 0; c < MAX_CONNS_PER_ATOM; c++) {
			    char conn = getAtom(feld [i] [j]).conn[c];
			    if (!conn)
				break;
			    
			    if (conn >= 'a' && conn <= 'a' + 8)
				bitBlt (this, x, y, &data, (conn - 'a') * 31, 31, 30, 30, XorROP);
			    else
				bitBlt (this, x, y, &data, (conn - 'A') * 31, 62, 30, 30, XorROP);
			    
			}
		    
		    // zeichnet Verbindungsstäbe 
		    if (getAtom(feld [i] [j]).obj >= 'A' && getAtom(feld [i] [j]).obj <= 'F')
			bitBlt (this, x, y, &data, (getAtom(feld [i] [j]).obj - 'A' + 2) * 31 , 93, 30, 30, 
				CopyROP);
		}
    }
    paint.end ();
}


#include "feld.moc"
