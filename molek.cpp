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



Molek::Molek( QWidget *parent, const char *name ) : QWidget( parent, name )
{
  level = 1;
  data.load ("molek.gif");

  loadFeldFromDat (1);
}

Molek::~Molek ()
{ 
}

void Molek::loadFeldFromDat (int l)
{
  int i, j;
  FILE *levelfile = stdin;
  level = l;

  l--;
  if ((levelfile = fopen ("fa.dat", "rb")) == NULL)
    debug ("Fehler beim Öffnen der Leveldatei (a.dat) !");

  fseek (levelfile, 975 * (long) l + 675, SEEK_SET);
  
  // moleküle einlesen  
  for (i = 0; i < 10; i++)
  for (j = 0; j < 10; j++)
  { 
    molek [i] [j].obj = fgetc (levelfile);
    molek [i] [j].verb = fgetc (levelfile);
    molek [i] [j].verb += (fgetc (levelfile) << 8);
  }
  fclose (levelfile);


  // name des moleküls auch noch lesen 
  if ((levelfile = fopen ("fn.dat", "rt")) == NULL)
    debug ("Fehler beim Öffnen der Namensdatei (n.dat) !");

  i = 0;
  while (i++ < l + 1)
    fgets (mname, 30, levelfile);
  fclose (levelfile);

  repaint ();
}

void Molek::paintEvent( QPaintEvent * )
{
  char st [15];
  int i, j, x, y;
  sprintf (st, "Level : %3d", level);
  QPainter paint (this);
  paint.setPen (QColor (190, 190, 190));
  paint.drawText (7, 152, mname, strlen (mname));
  paint.drawText (7, 170, st, strlen (st));
  // spielfeld gleich zeichnen 
  for (i = 0; i < 10; i++)
  for (j = 0; j < 10; j++)
  {
    x = 10 + i * 15;
    y = 10 + j * 15;

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
