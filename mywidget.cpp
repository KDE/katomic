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

// include options
#include "settings.h"

// include configbox
#include "configbox.h"

#include "mywidget.moc"

#include <qlayout.h>
#include <qgroupbox.h>
#include <kkeydialog.h>
#include <kstddirs.h>
#include <ksimpleconfig.h>

#define XPOS 10
#define YPOS 40

#define MPOSX 480
#define MPOSY 100

Options settings;

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
  options->insertItem(i18n("&Options"), this,
		      SLOT(configopts()) );

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

void MyWidget::configopts()
{
  (new ConfigBox(this, "Options"))->show();
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
  // moving keys
  accel->insertItem(i18n("Atom Up"), "up", "J");
  accel->insertItem(i18n("Atom Down"), "down", "N");
  accel->insertItem(i18n("Atom Left"), "left", "S");
  accel->insertItem(i18n("Atom Right"), "right", "D");
  accel->insertItem(i18n("Next Atom"), "down", "Space");

}


void MyWidget::showHighscores ()
{
  Highscore *h = new Highscore (this, "highscore", level, -1);  
  h->exec ();
  delete h;
}

void MyWidget::getButton (int button)
{
  feld->startAnimation ((Feld::Direction)button);
}

void MyWidget::gameOver(int moves) {
  QMessageBox::about (this, i18n("Congratulations"), i18n("You solved level %1 with %2 moves !").arg(level).arg(moves));
  // Messagebox öffnen, level gelöst
  
  Highscore high(this, "highscore", level, moves);
  high.exec ();
}

void MyWidget::getMoves(int moves)
{
  current.sprintf("%d", moves);
  ys->setText(current);
}

void MyWidget::updateLevel (int l)
{
  level = l;
  QString level = QString("levels/level_%1").arg(l);
  KSimpleConfig cfg(locate("appdata", level), true);
  cfg.setGroup("Level");
  feld->load(cfg);

  config = kapp->getConfig();
  level = QString("level%1").arg(l);
  config->setGroup(level);
  highest = config->readEntry("Moves0", "100");
  hs->setText(highest);

  ys->setText("");
}


void MyWidget::initConfig()
{
  config = kapp->getConfig();	

  /* CT this segfaults :-(
     accel->readSettings(config);
  */

  config->setGroup("Options");
  settings.anim_speed = config->readNumEntry("Animation Speed", 1);
  if (settings.anim_speed < 1 || settings.anim_speed > MAX_SPEED)
    settings.anim_speed = 1;

  config->setGroup("level1");
  highest = config->readEntry("Moves0", "100");

  config->setGroup("Colors");

  settings.changed = false;
}

void MyWidget::saveConfig()
{
  /* CT would this segfault too?
     accel->writeSettings(config);
  */

  if (settings.changed) {
    config->setGroup("Options");
    config->writeEntry("Animation Speed", settings.anim_speed);
    config->setGroup("Colors");
  }
  config->sync();

}


MyWidget::MyWidget ( QWidget *, const char* name )
  : KTMainWindow ( name )
{
  // 
  // setMaximumSize(665, 501);

  initConfig();
  
  level = 1;

  // molekül
  molek = new Molek (this, "molek");
  molek->setGeometry (MPOSX, MPOSY, 170, 180);
  molek->setBackgroundColor (QColor (0, 0, 0));

  // spielfeld    
  feld = new Feld (molek, this, "feld");
  feld->setGeometry (XPOS, YPOS, 15 * 30 + 1, 15 * 30 + 1);
  feld->setBackgroundColor( QColor( 0, 0, 0) );

  connect (feld, SIGNAL (gameOver(int)), SLOT(gameOver(int)));
  connect (feld, SIGNAL (sendMoves(int)), SLOT(getMoves(int)));

  // scrollbar       
  scrl = new QScrollBar(1, 67, 1, 5, 1, QScrollBar::Horizontal, this, "scrl" );
  scrl->setGeometry( MPOSX, 50, 160, 16 );

  connect (scrl, SIGNAL (valueChanged (int)), SLOT (updateLevel (int)));
  

  // the score group
  QGroupBox *bg = new QGroupBox ("Score", this, "bg");
  bg->setGeometry (MPOSX, 300, 160, 160);
  QBoxLayout *slay = new QVBoxLayout (bg, 10);
  
  slay->addSpacing(10);

  slay->addWidget(new QLabel(i18n("Highest score:"), bg));

  hs = new QLabel (highest, bg);
  hs->setAlignment(Qt::AlignRight);
  hs->setFont(QFont("Helvetica", 18, QFont::Bold));
  slay->addWidget(hs);

  slay->addSpacing(10);

  slay->addWidget(new QLabel(i18n("Your score so far:"), bg));
  
  ys = new QLabel (current, bg);
  ys->setAlignment(Qt::AlignRight);
  ys->setFont(QFont("Helvetica", 18, QFont::Bold));
  slay->addWidget(ys);
  
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
  QApplication::setColorSpec(QApplication::ManyColor);
  KApplication a(argc, argv, "katomic" );
  MyWidget *top;
  top = new MyWidget(0, "mywidget");
  a.setMainWidget(top);
  top->show();
  return a.exec();
}

