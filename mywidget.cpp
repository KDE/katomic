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


// enthält spielfeld
#include "feld.h"

// enthält molekül
#include "molek.h"

// enhält highscores
#include "highscore.h"


#include "mywidget.moc"

#include <kkeydialog.h>
#include <kstddirs.h>
#include <ksimpleconfig.h>

#define XPOS 10
#define YPOS 40

#define MPOSX 480
#define MPOSY 100

// ##########################
// #	class MyWidget      #
// ##########################

int level;

void MyWidget::createMenu()
{
  file = new QPopupMenu();
  file->insertItem (i18n ("&Highscores"),
                    this, SLOT (showHighscores ()));
  		
  file->insertSeparator(-1);
  file->insertItem(i18n("&Quit"), this, SLOT(quitapp()) );
	
  options = new QPopupMenu();
  options->insertItem(i18n("&Configure keys"), this,
                      SLOT(configkeys()) );

  help = new QPopupMenu();
  help->insertItem(i18n("&Help"), this, SLOT(helpmenu()) );
  help->insertItem(i18n("&About..."), this, SLOT(about()) );
	
  KMenuBar *menu = menuBar();
  menu->insertItem(i18n("&File"), file);
  menu->insertItem(i18n("&Options"), options);
  menu->insertSeparator(-1);
  menu->insertItem(i18n("&Help"), help);
  menuBar()->setGeometry(0, 0, width(), sizeHint().height());

}


void MyWidget::helpmenu()
{
	KApplication::getKApplication()->invokeHTMLHelp("","");
}

void MyWidget::configkeys()
{
  KKeyDialog::configureKeys(accel);
}


void MyWidget::initKeys()
{
  // here we create to shortcuts according to
  // the standard Kde keybinding
  accel = new KAccel(this);
	
  accel->insertItem(i18n("About"), "about", "CTRL+A");
  accel->connectItem("about", this, SLOT(about()) );
  accel->insertStdItem(KAccel::Help, i18n("Help"));
  accel->connectItem(KAccel::Help, this, SLOT(helpmenu()) );
  accel->insertStdItem(KAccel::Quit, i18n("Quit"));
  accel->connectItem(KAccel::Quit, this, SLOT(quitapp()) );
  accel->insertItem(i18n("Highscores"), "highscore", "CTRL+H");
  accel->connectItem ("highscore", this, 
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

void MyWidget::gameOver(int moves) {
  QMessageBox::about (this, i18n("Congratulations"), i18n("You solved level %1 with %2 moves !").arg(level).arg(moves));
  // Messagebox öffnen, level gelöst
  debug ("-------------- done -------------");
  
  Highscore high(this, "highscore", level, moves);
  high.exec ();
}

void MyWidget::updateLevel (int l)
{
  level = l;
  QString level = QString("levels/level_%1").arg(l);
  KSimpleConfig config(locate("appdata", level));
  config.setGroup("Level");
  feld->load(config);
  molek->load(config);
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
	: KTMainWindow ( name )
{
  // 
  // setMaximumSize(665, 501);

  initConfig();
  
  level = 1;

  // spielfeld    
  feld = new Feld (this, "feld");
  feld->setGeometry (XPOS, YPOS, 15 * 30 + 1, 15 * 30 + 1);
  feld->setBackgroundColor( QColor( 0, 0, 0) );

  connect (feld, SIGNAL (showDir ()), SLOT (showDir ()));
  connect (feld, SIGNAL (hideDir ()), SLOT (hideDir ()));
  connect (feld, SIGNAL (gameOver(int)), SLOT(gameOver(int)));

  // molekül
  molek = new Molek (this, "molek");
  molek->setGeometry (MPOSX, MPOSY, 170, 180);
  molek->setBackgroundColor (QColor (0, 0, 0));

  // scrollbar       
  scrl = new QScrollBar(1, 67, 1, 5, 1, QScrollBar::Horizontal, this, "scrl" );
  scrl->setGeometry( MPOSX, 50, 160, 16 );

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

  updateLevel(1);
  setMinimumSize(665, 501);
}

MyWidget::~MyWidget()
{

  delete file;
  delete options;
  delete help;
  delete feld;
}

void MyWidget::quitapp()
{
  saveConfig();
  kapp->quit();	
}

void MyWidget::about()
{
  QString q = i18n("Atomic 1.0.67 by Andreas Wüst (if0626@pc4.fh-isny.de)\n");
  KMsgBox::message(this, kapp->getCaption(), q );
}

// ##########################
// #	Main                #
// ##########################

int main(int argc, char **argv)
{
  KApplication a(argc, argv, "katomic" );
  MyWidget *top;
  top = new MyWidget(0, "mywidget");
  a.setMainWidget(top);
  top->show();
  return a.exec();
}

