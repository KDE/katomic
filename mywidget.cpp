/* mywidget.cpp
 
  Copyright (C) 1998   Andreas Wüst (if0626@pc4.fh-isny.de)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

  */


// bugs : no known bugs


#include "includes.h"

#include "defines.h"

// enthält spielfeld
#include "feld.h"

// enthält molekül
#include "molek.h"

// enhält highscores
#include "highscore.h"


#include "mywidget.moc"



#define XPOS 10
#define YPOS 40

#define MPOSX 480
#define MPOSY 100

KApplication *mywidget;



// ##########################
// #	class MyWidget      #
// ##########################


void MyWidget::createMenu()
{
  file = new QPopupMenu();
  file->insertItem (klocale->translate ("&Highscores"),
                    this, SLOT (showHighscores ()));
  		
  file->insertSeparator(-1);
  file->insertItem(klocale->translate("&Quit"), this, SLOT(quitapp()) );
	
  options = new QPopupMenu();
  options->insertItem(klocale->translate("&Configure keys"), this,
                      SLOT(configkeys()) );

  help = new QPopupMenu();
  help->insertItem(klocale->translate("&Help"), this, SLOT(helpmenu()) );
  help->insertItem(klocale->translate("&About..."), this, SLOT(about()) );
	
  menu = new KMenuBar(this, "menu");
  menu->insertItem(klocale->translate("&File"), file, 5);
  menu->insertItem(klocale->translate("&Options"), options);
  menu->insertSeparator(-1);
  menu->insertItem(klocale->translate("&Help"), help);

  setMenu(menu);
}


void MyWidget::helpmenu()
{
	KApplication::getKApplication()->invokeHTMLHelp("","");
}

void MyWidget::configkeys()
{
  kKeys->configureKeys(this);
}


void MyWidget::initKeys()
{
  // here we create to shortcuts according to
  // the standard Kde keybinding
  accel = new KStdAccel(config);
	
  kKeys->addKey("About", "CTRL+A");
  kKeys->addKey("Help", accel->help() );
  kKeys->addKey("Quit", accel->quit() );
  kKeys->addKey("Highscores", "CTRL+H");

  kKeys->registerWidget("top", this);
  kKeys->connectFunction ("top", "Help", this, SLOT(helpmenu()) );
  kKeys->connectFunction("top", "Quit", this, SLOT(quitapp()) );
  kKeys->connectFunction("top", "About", this, SLOT(about()) );
  kKeys->connectFunction ("top", "Highscores", this, 
                          SLOT (showHighscores ()));
}


void MyWidget::showHighscores ()
{
  Highscore *h = new Highscore (this, "highscore", level, -1);  
  h->exec ();
  delete h;
}

void MyWidget::showDir ()
{
  bg->show ();
}

void MyWidget::hideDir ()
{
  bg->hide ();
}

void MyWidget::getButton (int button)
{
  debug ("Button %d wurde gedrückt", button);
  if (button != 0)
    feld->startAnimation (button);
  else
    feld->done ();
}

void MyWidget::updateLevel (int l)
{
  level = l;
}


void MyWidget::initConfig()
{
  config = kapp->getConfig();	
  config->setGroup("Options");
  config->setGroup("Colors");
}

void MyWidget::saveConfig()
{
  config->setGroup("Options");
  config->setGroup("Colors");
  config->sync();
}


MyWidget::MyWidget ( QWidget *, const char* name )
	: KTopLevelWidget ( name )
{
  setCaption(CAPTION);

  setMinimumSize(665, 501);
  setMaximumSize(665, 501);

  initConfig();
  
  level = 1;

  // spielfeld    
  feld = new Feld (this, "feld");
  feld->setGeometry (XPOS, YPOS, 15 * 30 + 1, 15 * 30 + 1);
  feld->setBackgroundColor( QColor( 0, 0, 0) );

  connect (feld, SIGNAL (showDir ()), this, SLOT (showDir ()));
  connect (feld, SIGNAL (hideDir ()), this, SLOT (hideDir ()));

  // molekül
  molek = new Molek (this, "molek");
  molek->setGeometry (MPOSX, MPOSY, 170, 180);
  molek->setBackgroundColor (QColor (0, 0, 0));

  // scrollbar       
  scrl = new QScrollBar(1, 67, 1, 5, 1, QScrollBar::Horizontal, this, "scrl" );
  scrl->setGeometry( MPOSX, 50, 160, 16 );
  connect (scrl, SIGNAL (valueChanged (int)), feld, 
           SLOT (loadFeldFromDat (int)));
  connect (scrl, SIGNAL (valueChanged (int)), molek, 
           SLOT (loadFeldFromDat (int)));
  connect (scrl, SIGNAL (valueChanged (int)), SLOT (updateLevel (int)));
  

  // buttongroup
  bg = new QButtonGroup ("Direction", this, "bg");
  bg->setGeometry (MPOSX, 300, 160, 160);


  // buttons

  done = new QPushButton ("D", bg, "done");
  done->setGeometry (70, 70, 20, 20);

  up = new QPushButton ("UP", bg, "up");
  up->setGeometry (60, 20, 40, 40);

  right = new QPushButton ("RIGH", bg, "right");
  right->setGeometry (100, 60, 40, 40);
 
  down = new QPushButton ("DOWN", bg, "down");
  down->setGeometry (60, 100, 40, 40);

  left = new QPushButton ("LEFT", bg, "left");
  left->setGeometry (20, 60, 40, 40);


  connect (bg, SIGNAL (clicked (int)), SLOT (getButton (int)));

  
  hideDir ();  


  createMenu();
  initKeys();
}

MyWidget::~MyWidget()
{

  delete file;
  delete options;
  delete help;
  delete menu;
         
 
  delete feld;
}

void MyWidget::quitapp()
{
  saveConfig();
  mywidget->quit();	
}

void MyWidget::about()
{
  QString q;
  const char *msg = klocale->translate("\nAtomic 1.0.67\n\n");

  q.sprintf("%sby Andreas Wüst (if0626@pc4.fh-isny.de)\n", msg);
  KMsgBox::message(this, CAPTION, q );
}


void MyWidget::notImpl()
{
  KMsgBox::message(this, CAPTION,
  klocale->translate("Sorry, this function hasn't been implemented yet..."));
}


// ##########################
// #	Main                #
// ##########################


int main(int argc, char **argv)
{
  mywidget = new KApplication(argc, argv, "mywidget" );
  MyWidget *top;
  top = new MyWidget(0, "mywidget");
  mywidget->setMainWidget(top);
  top->show();
  return mywidget->exec();
}

