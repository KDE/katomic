/****************************************************************
**
** Implementation Feld class, derieved from Qt tutorial 8
**
****************************************************************/

// bemerkungen : wenn paintEvent aufgerufen wird, wird das komplette
//               widget gel�scht und nur die sachen gezeichnet, die in 
//               paintEvent stehen ! sollen dinge z.b nur bei maustasten-
//               druck gezeichnet werden, so mu� dies in mousePressEvent 
//               stehen !  
//               paintEvent wird aufgerufen, falls fenster �berdeckt wird,
//               oder auch einfach bewegt wird 



#include "feld.moc"

#include "highscore.h"



Feld::Feld( QWidget *parent, const char *name ) : QWidget( parent, name )
{
  level = 1;
  anim = FALSE;
  dir = 0;
  speed = 1;
  sprite = QPixmap (30, 30);

  cx = -1;
  cy = -1;

  point = new QPoint [1];
  data.load ("abilder.gif");
 
  moving = FALSE;
  pressed = FALSE;
  chosen = FALSE;

  loadFeldFromDat (1);

  // mousemoveevent wird aufgerufen, sobald sich maus in feld befindet
  // und pos sich ver�ndert
  setMouseTracking (TRUE);
}

Feld::~Feld ()
{ 
  delete [] point;
}

void Feld::loadFeldFromDat (int l)
{
  debug ("in loadfeld");
  int i, j;
  FILE *levelfile = stdin;
 
  level = l;
  debug ("level : %d", level);
  l--;
  if ((levelfile = fopen ("fa.dat", "rb")) == NULL)
    debug ("Fehler beim �ffnen der Leveldatei (a.dat) !");

  fseek (levelfile, 975 * (long) l, SEEK_SET);

  for (i = 0; i < 15; i++)
  for (j = 0; j < 15; j++)
  {
    feld [i] [j].obj = fgetc (levelfile);
    feld [i] [j].verb = fgetc (levelfile);
    feld [i] [j].verb += (fgetc (levelfile) << 8);
  }
  
  //  fread doesn't work again, why ?
  //  fread (&molek [0] [0], sizeof (molek), 1, levelfile);
  
  for (i = 0; i < 10; i++)
  for (j = 0; j < 10; j++)
  { 
    molek [i] [j].obj = fgetc (levelfile);
    molek [i] [j].verb = fgetc (levelfile);
    molek [i] [j].verb += (fgetc (levelfile) << 8);
  }
  fclose (levelfile);

  // h�he und breite des molek�ls berechnen und ausgeben, checkdone 
  for (i = 0, breite = 0, hohe = 0; i < 10; i++)
  for (j = 0; j < 10; j++)
  {
    if ((molek [i] [j].verb != 0) && (i > breite)) breite = i;
    if ((molek [i] [j].verb != 0) && (j > hohe)) hohe = j;
  }
  
  moves = 0;
  repaint ();
}



void Feld::mousePressEvent (QMouseEvent *e)
{
  int x, y;
  debug ("chosen : %d", chosen);
  if (moving == TRUE) 
    return;

  x = e->pos ().x () / 30;
  y = e->pos ().y () / 30;

 

  // cursor sichtbar, feld ausgew�hlt
  if (pressed == FALSE)
  {
    if (feld [x] [y].obj != 254 && feld [x] [y].obj != 0)
    {
      debug ("x : %d, y : %d", x, y);
      setCursor (blankCursor);
      pressed = TRUE;
      chosen = TRUE;
      xpos = x;
      ypos = y;
      dir = 0;
      // cursor in mitte von feld stellen 
      QCursor::setPos (mapToGlobal (QPoint (550, 340)));
      emit (showDir ());
    }
  }

  // cursor ist unsichtbar, soll wieder sichtbar gemacht werden,
  // um neues feld w�hlen zu k�nnen
  else
  if (pressed == TRUE)
  {

  }

}
  

void Feld::mouseReleaseEvent (QMouseEvent *)
{
}


void Feld::done ()
{
  if (moving == FALSE)
  {
    QCursor::setPos (mapToGlobal (QPoint (xpos * 30 + 15, ypos * 30 + 15)));
 
    pressed = FALSE;
    chosen = FALSE;
    setCursor (crossCursor);
    emit (hideDir ());
    if (checkDone () == TRUE)
    {
      char st1 [] = {"Congratulations"}, st2 [80];
      sprintf (st2, "Sie haben den %d. Level\nmit %d Z�gen gel�st !", level, moves);
      QMessageBox::about (this, st1, st2);
      // Messagebox �ffnen, level gel�st
      debug ("-------------- done -------------");
   
      debug ("moves %d, level  %d", moves, level);


      high = new Highscore (this, "highscore", level, moves);
      high->exec ();
    
      delete high;
      

      loadFeldFromDat (level);
    }
  }
}

void Feld::startAnimation (int d)
{
  int x = 0, y = 0;
 
  // wenn bereits animation stattfindet, nix machen
  if (moving == TRUE)
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
    moving = TRUE;
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
 
  if (chosen == FALSE)
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

  // alles au�er den atomen l�schen 
  for (i = 0; i < 15; i++)
  for (j = 0; j < 15; j++)
  if (f [i] [j].obj == 254) 
  {
    memset (&f [i] [j], 0, 3);
  }  
    
  // f und molek vergleichen   
  for (i = 0; i < 15 - breite; i++)
  for (j = 0; j < 15 - hohe; j++)
  {
    done = TRUE;
    if ((f [i] [j].obj == molek [0] [0].obj) && (f [i] [j].verb == molek [0] [0].verb))        // gleich links oben
    {
      for (xx = 0; xx < breite + 1; xx++)
      for (yy = 0; yy < hohe + 1; yy++)
      {
        // ersten 3 bytes des strukturelements vergleichen (obj, verb)
        if ((strncmp ((const char*)&f [i + xx] [j + yy], 
                      (const char*)&molek [xx] [yy], 3)) != 0) 
          done = FALSE;
      }
      if (done == TRUE)
        return (TRUE);
    }
  }
  return (FALSE);
}


void Feld::timerEvent (QTimerEvent *)
{
  // animation beenden 
  if (frames < 1)
  {
    moving = FALSE;
    killTimers ();
    debug ("done");
    dir = 0;
  }
  else
  {  
    frames -= speed;
    repaint (FALSE);
  }
}


void Feld::paintEvent( QPaintEvent * )
{
  int i, j, x, y;

  QPainter paint (this);

  paint.setPen (black);
  

  if (moving == TRUE)
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
      // zeichnet Randst�cke
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
          if ((feld [i] [j].verb & (1 << anz)) == (1 << anz))
          {
            if (anz < 8)
              bitBlt (this, x, y, &data, anz * 31, 31, 30, 30, XorROP);
            else
              bitBlt (this, x, y, &data, (anz - 8) * 31, 62, 30, 30, XorROP);
          }
      }

  
    // zeichnet Verbindungsst�be 
      if (feld [i] [j].obj > 29 && feld [i] [j].obj < 34)
        bitBlt (this, x, y, &data, (feld [i] [j].obj - 28) * 31 , 93, 30, 30, 
                CopyROP);
    }
  }  
  paint.end ();
}
