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



#include "feld.moc"

#include "highscore.h"
#include <kiconloader.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <ksimpleconfig.h>

int hexValue(char ch) {
  if (ch >= '0' && ch <= '9')
    return ch - '0';
  if (ch >= 'a' && ch <= 'f')
    return ch - 'a' + 10;
  if (ch >= 'A' && ch <= 'F')
    return ch - 'A' + 10;
  // fatal("found %c", ch);
  return 0;
}

Feld::Feld( QWidget *parent, const char *name ) : QWidget( parent, name )
{
  level = 1;
  anim = false;
  dir = 0;
  speed = 1;
  sprite = QPixmap (30, 30);

  cx = -1;
  cy = -1;

  point = new QPoint [1];
  data = ICON("abilder.png");
 
  moving = false;
  pressed = false;
  chosen = false;

  // mousemoveevent wird aufgerufen, sobald sich maus in feld befindet
  // und pos sich verändert
  setMouseTracking (true);
}

Feld::~Feld ()
{ 
  delete [] point;
}

void Feld::loadFeldFromDat (const KSimpleConfig& config)
{
  debug ("in loadfeld");
 
  QString key;
  
  for (int i = 0; i < 15; i++) {
    
    key = QString("feld_obje_%1").arg(i);
    const QString obj_line = config.readEntry(key);
    key = QString("feld_verb_%1").arg(i);
    const QString verb_line = config.readEntry(key);

    int verb_index = 0;
    int obj_index = 0;

    for (int j = 0; j < 15; j++)
      {
	feld [i] [j].obj = 
	  hexValue(obj_line.at(obj_index++)) * 16 + 
	  hexValue(obj_line.at(obj_index++));

	feld [i][j].verb = 
	  hexValue(verb_line.at(verb_index++)) * 4096 + 
	  hexValue(verb_line.at(verb_index++)) * 265+ 
	  hexValue(verb_line.at(verb_index++)) * 16 + 
	  hexValue(verb_line.at(verb_index++));
	
      }
  }

#if 0
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

#endif 

  moves = 0;
  repaint ();
}



void Feld::mousePressEvent (QMouseEvent *e)
{
  int x, y;
  debug ("chosen : %d", chosen);
  if (moving == true) 
    return;

  x = e->pos ().x () / 30;
  y = e->pos ().y () / 30;

 

  // cursor sichtbar, feld ausgewählt
  if (pressed == false)
  {
    if (feld [x] [y].obj != 254 && feld [x] [y].obj != 0)
    {
      debug ("x : %d, y : %d", x, y);
      setCursor (blankCursor);
      pressed = true;
      chosen = true;
      xpos = x;
      ypos = y;
      dir = 0;
      // cursor in mitte von feld stellen 
      QCursor::setPos (mapToGlobal (QPoint (550, 340)));
      emit (showDir ());
    }
  }

  // cursor ist unsichtbar, soll wieder sichtbar gemacht werden,
  // um neues feld wählen zu können
  else
  if (pressed == true)
  {

  }

}
  

void Feld::mouseReleaseEvent (QMouseEvent *)
{
}


void Feld::done ()
{
  if (moving == false)
  {
    QCursor::setPos (mapToGlobal (QPoint (xpos * 30 + 15, ypos * 30 + 15)));
 
    pressed = false;
    chosen = false;
    setCursor (crossCursor);
    emit (hideDir ());
    if (checkDone () == true)
      emit gameOver(moves);
  }
}

void Feld::startAnimation (int d)
{
  int x = 0, y = 0;
 
  // wenn bereits animation stattfindet, nix machen
  if (moving == true)
    return;

  moves++;
  dir = d;
  
  switch (dir)
  {      
    case 1 : for (x = xpos, y = ypos, anz = 0; feld [x] [--y].obj == 0; anz++);
             if (anz != 0)
             {
               feld [x] [++y] = feld [xpos] [ypos];
             }
             break;
    case 3 : debug ("unten");
             for (x = xpos, y = ypos, anz = 0; feld [x] [++y].obj == 0; anz++);
             if (anz != 0)
             {
               feld [x] [--y] = feld [xpos] [ypos];
             }
             break;
    case 2 : for (x = xpos, y = ypos, anz = 0; feld [++x] [y].obj == 0; anz++);
             if (anz != 0)
             {
               feld [--x] [y] = feld [xpos] [ypos];
             }
             break;
    case 4 : for (x = xpos, y = ypos, anz = 0; feld [--x] [y].obj == 0; anz++);
             if (anz != 0)
             { 
               feld [++x] [y] = feld [xpos] [ypos];
	     }
  }

  if (anz != 0)
  {
    moving = true;
    feld [xpos] [ypos].obj = 0;
    feld [xpos] [ypos].verb = 0;

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
  int x, y;
 
  if (chosen == false)
  {   
    x = e->pos ().x () / 30;
    y = e->pos ().y () / 30;

    // verschiedene cursor je nach pos
    if (feld [x] [y].obj != 254 && feld [x] [y].obj != 0)
      setCursor (crossCursor);
    else
      setCursor (arrowCursor);
    //    debug ("chosen : %d", chosen);
    //    debug ("%d %d", e->pos (). x(), e->pos (). y ());
  }

  
}


bool Feld::checkDone ()
{
  struct spielfeld f [15] [15];
  unsigned char i, j, xx, yy;
  bool done;
  memcpy (f, feld, sizeof (feld));

  // alles außer den atomen löschen 
  for (i = 0; i < 15; i++)
  for (j = 0; j < 15; j++)
  if (f [i] [j].obj == 254) 
  {
    memset (&f [i] [j], 0, 3);
  }  

#if 0    
  // f und molek vergleichen   
  for (i = 0; i < 15 - breite; i++)
  for (j = 0; j < 15 - hohe; j++)
  {
    done = true;
    if ((f [i] [j].obj == molek [0] [0].obj) && (f [i] [j].verb == molek [0] [0].verb))        // gleich links oben
    {
      for (xx = 0; xx < breite + 1; xx++)
      for (yy = 0; yy < hohe + 1; yy++)
      {
        // ersten 3 bytes des strukturelements vergleichen (obj, verb)
        if ((strncmp ((const char*)&f [i + xx] [j + yy], 
                      (const char*)&molek [xx] [yy], 3)) != 0) 
          done = false;
      }
      if (done == true)
        return (true);
    }
  }
#endif
  return false;
}


void Feld::timerEvent (QTimerEvent *)
{
  // animation beenden 
  if (frames < 1)
  {
    moving = false;
    killTimers ();
    debug ("done");
    dir = 0;
  }
  else
  {  
    frames -= speed;
    repaint (false);
  }
}


void Feld::paintEvent( QPaintEvent * )
{
  int i, j, x, y;

  QPainter paint (this);

  paint.setPen (black);
  

  if (moving == true)
  {
    switch (dir)
    {
      case 1 : bitBlt (this, cx, cy - framesbak + frames, &sprite, CopyROP);
               if ( (framesbak - frames > 1)  )
	       {
		 //  debug ("framesbak %d, frames %d", framesbak, frames);
                 paint.eraseRect (cx, cy - framesbak + frames + 30, 30, 2);
	       }
               break;
      case 3 : bitBlt (this, cx, cy + (framesbak - frames), &sprite, CopyROP);
               if ( (framesbak - frames > 1) )
	       {
                 paint.eraseRect (cx, cy + (framesbak - frames) - 2, 30, 2);
	       }
               break;
      case 2 : bitBlt (this, cx + (framesbak - frames), cy, &sprite, CopyROP);
               if ( (framesbak - frames > 1) )
	       {	 
                 paint.eraseRect (cx + (framesbak - frames) - 2, cy, 2, 30);
	       }
               break;
      case 4 : bitBlt (this, cx - framesbak + frames, cy, &sprite, CopyROP);
               if ((framesbak - frames > 1))
	       {
                 paint.eraseRect (cx - framesbak + frames + 30, cy, 2, 30);
	       }

    }

  }

 
  else 
  {
    // spielfeld gleich zeichnen 

    for (i = 0; i < 15; i++)
    for (j = 0; j < 15; j++)
    {
      x = i * 30;
      y = j * 30;
      // zeichnet Randstücke
      if (feld [i] [j].obj == 254) 
        bitBlt (this, x, y, &data, 279, 31, 30, 30, CopyROP);
  
      // zeichnet Atome
      if (feld [i] [j].obj < 11 && feld [i] [j].obj > 0)
      {
        bitBlt (this, x, y, &data, (feld [i] [j].obj - 1) * 31, 0, 30, 
                30, CopyROP);
      }
  
      // zeichnet Kristalle
      if (feld [i] [j].obj == 20)
      {
        bitBlt (this, x, y, &data, 31, 93, 30, 30, CopyROP);
      }
  
      // verbindungen zeichnen
      if (feld [i] [j].obj < 11 || feld [i] [j].obj == 20)
      {
        char anz;
        for (anz = 0; anz < 16; anz++)
          if ((feld [i] [j].verb & (1 << anz)) == (uint(1) << anz))
          {
            if (anz < 8)
              bitBlt (this, x, y, &data, anz * 31, 31, 30, 30, XorROP);
            else
              bitBlt (this, x, y, &data, (anz - 8) * 31, 62, 30, 30, XorROP);
          }
      }

  
    // zeichnet Verbindungsstäbe 
      if (feld [i] [j].obj > 29 && feld [i] [j].obj < 34)
        bitBlt (this, x, y, &data, (feld [i] [j].obj - 28) * 31 , 93, 30, 30, 
                CopyROP);
    }
  }  
  paint.end ();
}
